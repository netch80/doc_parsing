public class MyVisitor extends gr04BaseVisitor<SemanticValue> {
    public int verbose = 0;
    ExecutionContext econtext = null;

    @Override
    public SemanticValue visitStmt1(gr04Parser.Stmt1Context ctx) {
        return visit(ctx.stmt_defmap());
    }

    @Override
    public SemanticValue visitStmt0(gr04Parser.Stmt0Context ctx) {
        return visit(ctx.expression());
    }

    @Override
    public SemanticValue visitStmt_defmap(gr04Parser.Stmt_defmapContext ctx) {
        String ident = ctx.IDENTIFIER().getText();
        econtext.createMap(ident);
        return null;
    }

    @Override
    public SemanticValue visitExpression(gr04Parser.ExpressionContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitExpression\n");
        }
        return visit(ctx.assignment());
    }

    @Override
    public SemanticValue visitAsg0(gr04Parser.Asg0Context ctx) {
        return visit(ctx.addsub());
    }

    @Override
    public SemanticValue visitAsg1(gr04Parser.Asg1Context ctx) {
        SemanticValue lvalue = visit(ctx.primary());
        Double rvalue = visit(ctx.assignment()).getRValue(econtext);
        lvalue.assignValue(rvalue, econtext);
        if (verbose > 0) {
            System.out.printf("__: visitAsg1: rvalue=%s\n", rvalue);
        }
        return new ScalarSV(rvalue);
    }

    @Override
    public SemanticValue visitAs0(gr04Parser.As0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitAs0\n");
        }
        return visit(ctx.muldiv());
    }

    @Override
    public SemanticValue visitAsplus(gr04Parser.AsplusContext ctx) {
        double d1 = visit(ctx.addsub()).getRValue(econtext);
        double d2 = visit(ctx.muldiv()).getRValue(econtext);
        return new ScalarSV(d1 + d2);
    }

    @Override
    public SemanticValue visitAsminus(gr04Parser.AsminusContext ctx) {
        double d1 = visit(ctx.addsub()).getRValue(econtext);
        double d2 = visit(ctx.muldiv()).getRValue(econtext);
        return new ScalarSV(d1 - d2);
    }

    @Override
    public SemanticValue visitMd0(gr04Parser.Md0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitMd0\n");
        }
        return visit(ctx.power());
    }

    @Override
    public SemanticValue visitMdmul(gr04Parser.MdmulContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitMdmul\n");
        }
        double d1 = visit(ctx.muldiv()).getRValue(econtext);
        double d2 = visit(ctx.power()).getRValue(econtext);
        return new ScalarSV(d1 * d2);
    }

    @Override
    public SemanticValue visitMddiv(gr04Parser.MddivContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitMddiv\n");
        }
        double d1 = visit(ctx.muldiv()).getRValue(econtext);
        double d2 = visit(ctx.power()).getRValue(econtext);
        return new ScalarSV(d1 / d2);
    }

    @Override
    public SemanticValue visitPwr0(gr04Parser.Pwr0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitPwr0\n");
        }
        return visit(ctx.unary());
    }

    @Override
    public SemanticValue visitPwrpwr(gr04Parser.PwrpwrContext ctx) {
        double d1 = visit(ctx.unary()).getRValue(econtext);
        double d2 = visit(ctx.power()).getRValue(econtext);
        return new ScalarSV(Math.pow(d1, d2));
    }

    @Override
    public SemanticValue visitUn0(gr04Parser.Un0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitUn0\n");
        }
        return visit(ctx.primary());
    }

    @Override
    public SemanticValue visitUnplus(gr04Parser.UnplusContext ctx) {
        return visit(ctx.unary());
    }

    @Override
    public SemanticValue visitUnminus(gr04Parser.UnminusContext ctx) {
        return new ScalarSV(-visit(ctx.unary()).getRValue(econtext));
    }

    @Override
    public SemanticValue visitPrimexpr(gr04Parser.PrimexprContext ctx) {
        return visit(ctx.expression());
    }

    @Override
    public SemanticValue visitPrimindex(gr04Parser.PrimindexContext ctx) {
        SemanticValue v1 = new IdentifierSV(ctx.IDENTIFIER().getText());
        if (verbose > 0) {
            System.out.printf("__: visitPrimindex: v1.class=%s\n",
                    v1.getClass().getName());
        }
        IdentifierSV ident = (IdentifierSV) v1;
        SemanticValue v2 = visit(ctx.expression());
        return new IndexedReferenceSV(ident, v2);
    }

    @Override
    public SemanticValue visitPrimatom(gr04Parser.PrimatomContext ctx) {
        return visit(ctx.atom());
    }

    @Override
    public SemanticValue visitAtomnum(gr04Parser.AtomnumContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitAtomnum\n");
        }
        return new ScalarSV(Double.valueOf(ctx.NUMBER().getText()));
    }

    @Override
    public SemanticValue visitAtomid(gr04Parser.AtomidContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitAtomid\n");
        }
        return new IdentifierSV(ctx.getText());
    }
}
