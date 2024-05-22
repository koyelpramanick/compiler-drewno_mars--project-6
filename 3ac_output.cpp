#include "ast.hpp"

namespace drewno_mars{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

void FnDeclNode::to3AC(IRProgram * prog){
	SemSymbol * symbol = myID->getSymbol();
    Procedure * proc = prog->makeProc(symbol->getName());

    // turn all formals into 3AC
    for(auto formal : *myFormals) {
        formal->to3AC(proc);
    }

    //Add a quad for every formal
    size_t index = 1;
    for (auto formal : *myFormals){
        SemSymbol * sym = formal->ID()->getSymbol();
        SymOpd * opd = proc->getSymOpd(sym);

        proc->addQuad(new GetArgQuad(index, opd));
        index++;
    }

    //generate 3AC for each stmt
    for(auto stmt : *myBody) {
        stmt->to3AC(proc);
    }

    //functions are globals
    prog->gatherGlobal(symbol);
}

void FnDeclNode::to3AC(Procedure * proc){
	//This never needs to be implemented,
	// the function only exists because of
	// inheritance needs (A function declaration
	// never occurs within another function)
	throw new InternalError("FnDecl at a local scope");
}

void FormalDeclNode::to3AC(IRProgram * prog){
	//This never needs to be implemented,
	// the function only exists because of
	// inheritance needs (A formal never
	// occurs at global scope)
	throw new InternalError("Formal at a global scope");
}

void FormalDeclNode::to3AC(Procedure * proc){
    SemSymbol * symbol = ID()->getSymbol();
    assert(symbol != nullptr);
    proc->gatherFormal(symbol);
}

Opd * IntLitNode::flatten(Procedure * proc){
	const DataType * type = proc->getProg()->nodeType(this);
	return new LitOpd(std::to_string(myNum), 8);
}

Opd * StrLitNode::flatten(Procedure * proc){
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * TrueNode::flatten(Procedure * proc){
    return new LitOpd("1", 8);
}

Opd * FalseNode::flatten(Procedure * proc){
    return new LitOpd("0", 8);
}

Opd * CallExpNode::flatten(Procedure * proc){
    size_t index = 1;
	for (auto arg: *myArgs) {
        Opd * exp = arg->flatten(proc);
        proc->addQuad(new SetArgQuad(index, exp));
        index++;
    }

    SemSymbol * symbol = myCallee->getSymbol();

    proc->addQuad(new CallQuad(symbol));

    //if the return type is void
    if(symbol->getDataType()->isVoid()) {
        return nullptr;
    } else {
        Opd * returnValue = proc->makeTmp(8);
        proc->addQuad(new GetRetQuad(returnValue));
        return returnValue;
    }

}

Opd * NegNode::flatten(Procedure * proc){
	Opd * src = myExp->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new UnaryOpQuad(dst, NEG64, src));
    return dst;
}

Opd * NotNode::flatten(Procedure * proc){
    Opd * src = myExp->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new UnaryOpQuad(dst, NOT64, src));
    return dst;
}

Opd * PlusNode::flatten(Procedure * proc){
	Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, ADD64, src1, src2));
    return dst;
}

Opd * MinusNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, SUB64, src1, src2));
    return dst;
}

Opd * TimesNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, MULT64, src1, src2));
    return dst;
}

Opd * DivideNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, DIV64, src1, src2));
    return dst;
}

Opd * AndNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, AND64, src1, src2));
    return dst;
}

Opd * OrNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, OR64, src1, src2));
    return dst;
}

Opd * EqualsNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, EQ64, src1, src2));
    return dst;
}

Opd * NotEqualsNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, NEQ64, src1, src2));
    return dst;
}

Opd * LessNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, LT64, src1, src2));
    return dst;
}

Opd * GreaterNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, GT64, src1, src2));
    return dst;
}

Opd * LessEqNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, LTE64, src1, src2));
    return dst;
}

Opd * GreaterEqNode::flatten(Procedure * proc){
    Opd * src1 = myExp1->flatten(proc);
    Opd * src2 = myExp2->flatten(proc);
    Opd * dst = proc->makeTmp(8);
    proc->addQuad(new BinOpQuad(dst, GTE64, src1, src2));
    return dst;
}

void AssignStmtNode::to3AC(Procedure * proc){
	Opd * src = mySrc->flatten(proc);
    Opd * dst = myDst->flatten(proc);
    assert(dst != nullptr);
    proc->addQuad(new AssignQuad(dst, src));
}

void PostIncStmtNode::to3AC(Procedure * proc){
    Opd * dst = myLoc->flatten(proc);
    proc->addQuad(new BinOpQuad(dst, ADD64, dst, new LitOpd("1", 8)));
}

void PostDecStmtNode::to3AC(Procedure * proc){
    Opd * dst = myLoc->flatten(proc);
    proc->addQuad(new BinOpQuad(dst, SUB64, dst, new LitOpd("1", 8)));
}

void GiveStmtNode::to3AC(Procedure * proc){
	Opd * src = mySrc->flatten(proc);
    proc->addQuad(new WriteQuad(src));
}

void TakeStmtNode::to3AC(Procedure * proc){
    Opd * dst = myDst->flatten(proc);
    proc->addQuad(new ReadQuad(dst));
}

void IfStmtNode::to3AC(Procedure * proc){
	Opd * exp = myCond->flatten(proc);
    Label * tgt = proc->makeLabel();
    proc->addQuad(new IfzQuad(exp, tgt));

    for(auto stmt : *myBody) {
        stmt->to3AC(proc);
    }

    Quad * quad = new NopQuad();
    proc->addQuad(quad);
    quad->addLabel(tgt);

}

void IfElseStmtNode::to3AC(Procedure * proc){
    Opd * exp = myCond->flatten(proc);
    Label * elseStmt = proc->makeLabel();
    Label * tgt = proc->makeLabel();
    proc->addQuad(new IfzQuad(exp, elseStmt));

    for(auto stmt : *myBodyTrue) {
        stmt->to3AC(proc);
    }
    proc->addQuad(new GotoQuad(tgt));

    Quad * elseQuad = new NopQuad();
    proc->addQuad(elseQuad);
    elseQuad->addLabel(elseStmt);
    for(auto stmt : *myBodyFalse) {
        stmt->to3AC(proc);
    }

    Quad * endQuad = new NopQuad();
    proc->addQuad(endQuad);
    endQuad->addLabel(tgt);
}

void WhileStmtNode::to3AC(Procedure * proc){
    Label * head = proc->makeLabel();
    Label * end = proc->makeLabel();

    Quad * ifLabel = new NopQuad();
    proc->addQuad(ifLabel);
    ifLabel->addLabel(head);

    Opd * exp = myCond->flatten(proc);
    Quad * ifQuad = new IfzQuad(exp, end);
    proc->addQuad(ifQuad);

    for(auto stmt : *myBody) {
        stmt->to3AC(proc);
    }

    proc->addQuad(new GotoQuad(head));

    Quad * quad = new NopQuad();
    proc->addQuad(quad);
    quad->addLabel(end);
}

void CallStmtNode::to3AC(Procedure * proc){
    Opd * callExp = myCallExp->flatten(proc);

    assert(callExp != nullptr);
    proc->popQuad();
}

void ReturnStmtNode::to3AC(Procedure * proc){
    if(myExp != nullptr) {
        Opd * exp = myExp->flatten(proc);
        proc->addQuad(new SetRetQuad(exp));
    }
    Label * returnLabel = proc->getLeaveLabel();
    proc->addQuad(new GotoQuad(returnLabel));
}

void ExitStmtNode::to3AC(Procedure * proc){
    // I don't think this does anything
}

void VarDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	proc->gatherLocal(sym);
}

void VarDeclNode::to3AC(IRProgram * prog){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	prog->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent)
Opd * IDNode::flatten(Procedure * proc){
        SemSymbol * symbol = this->getSymbol();
        assert(symbol != nullptr);
        return proc->getSymOpd(symbol);
}

}
