public class MyVisitor extends gr02BaseVisitor<Double> {
    public int verbose = 0;

    @Override
    public Double visitExpression(gr02Parser.ExpressionContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitExpression\n");
        }
        return visit(ctx.addsub());
    }

    @Override
    public Double visitAs0(gr02Parser.As0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitAs0\n");
        }
        return visit(ctx.muldiv());
    }

    @Override
    public Double visitAsplus(gr02Parser.AsplusContext ctx) {
        return visit(ctx.addsub()) + visit(ctx.muldiv());
    }

    @Override
    public Double visitAsminus(gr02Parser.AsminusContext ctx) {
        return visit(ctx.addsub()) - visit(ctx.muldiv());
    }

    @Override
    public Double visitMd0(gr02Parser.Md0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitMd0\n");
        }
        return visit(ctx.power());
    }

    @Override
    public Double visitMdmul(gr02Parser.MdmulContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitMdmul\n");
        }
        return visit(ctx.muldiv()) * visit(ctx.power());
    }

    @Override
    public Double visitMddiv(gr02Parser.MddivContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitMddiv\n");
        }
        return visit(ctx.muldiv()) / visit(ctx.power());
    }

    @Override
    public Double visitPwr0(gr02Parser.Pwr0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitPwr0\n");
        }
        return visit(ctx.unary());
    }

    @Override
    public Double visitPwrpwr(gr02Parser.PwrpwrContext ctx) {
        return Math.pow( visit(ctx.unary()), visit(ctx.power()));
    }

    @Override
    public Double visitUn0(gr02Parser.Un0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitUn0\n");
        }
        return visit(ctx.atom());
    }

    @Override
    public Double visitUnplus(gr02Parser.UnplusContext ctx) {
        return visit(ctx.unary());
    }

    @Override
    public Double visitUnminus(gr02Parser.UnminusContext ctx) {
        return -visit(ctx.unary());
    }

    @Override
    public Double visitAtomexpr(gr02Parser.AtomexprContext ctx) {
        return visit(ctx.expression());
    }

    @Override
    public Double visitAtomnum(gr02Parser.AtomnumContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitAtomnum\n");
        }
        return Double.valueOf(ctx.NUMBER().getText());
    }
}
