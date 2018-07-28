public class MyVisitor extends gr03BaseVisitor<Double> {
    public int verbose = 0;

    @Override
    public Double visitExpression(gr03Parser.ExpressionContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitExpression\n");
        }
        return visit(ctx.addsub());
    }

    @Override
    public Double visitAs0(gr03Parser.As0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitAs0\n");
        }
        return visit(ctx.muldiv());
    }

    @Override
    public Double visitAsplus(gr03Parser.AsplusContext ctx) {
        return visit(ctx.addsub()) + visit(ctx.muldiv());
    }

    @Override
    public Double visitAsminus(gr03Parser.AsminusContext ctx) {
        return visit(ctx.addsub()) - visit(ctx.muldiv());
    }

    @Override
    public Double visitMd0(gr03Parser.Md0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitMd0\n");
        }
        return visit(ctx.unary());
    }

    @Override
    public Double visitMdmul(gr03Parser.MdmulContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitMdmul\n");
        }
        return visit(ctx.muldiv()) * visit(ctx.unary());
    }

    @Override
    public Double visitMddiv(gr03Parser.MddivContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitMddiv\n");
        }
        return visit(ctx.muldiv()) / visit(ctx.unary());
    }

    @Override
    public Double visitUn0(gr03Parser.Un0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitUn0\n");
        }
        return visit(ctx.power());
    }

    @Override
    public Double visitUnplus(gr03Parser.UnplusContext ctx) {
        return visit(ctx.unary());
    }

    @Override
    public Double visitUnminus(gr03Parser.UnminusContext ctx) {
        return -visit(ctx.unary());
    }

    @Override
    public Double visitPwr0(gr03Parser.Pwr0Context ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitPwr0\n");
        }
        return visit(ctx.atom());
    }

    @Override
    public Double visitPwrpwr(gr03Parser.PwrpwrContext ctx) {
        return Math.pow( visit(ctx.atom()), visit(ctx.unary()));
    }

    @Override
    public Double visitAtomexpr(gr03Parser.AtomexprContext ctx) {
        return visit(ctx.expression());
    }

    @Override
    public Double visitAtomnum(gr03Parser.AtomnumContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitAtomnum\n");
        }
        return Double.valueOf(ctx.NUMBER().getText());
    }
}
