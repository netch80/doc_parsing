import org.antlr.v4.runtime.tree.TerminalNodeImpl;
import org.antlr.v4.runtime.tree.ParseTree;

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
    public Double visitAddsub(gr02Parser.AddsubContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitAddsub\n");
        }
        if (ctx.children.size() == 3) {
            Double v1 = visit(ctx.getChild(0));
            Double v2 = visit(ctx.getChild(2));
            // NB we can call getText() and compare token text :)
            TerminalNodeImpl op = (TerminalNodeImpl) ctx.getChild(1);
            int optype = op.getSymbol().getType();
            if (optype == gr02Lexer.PLUS) {
                return v1 + v2;
            } else {
                return v1 - v2;
            }
        } else {
            return visit(ctx.getChild(0));
        }
    }

    @Override
    public Double visitMuldiv(gr02Parser.MuldivContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitMuldiv\n");
        }
        if (ctx.children.size() == 3) {
            Double v1 = visit(ctx.getChild(0));
            Double v2 = visit(ctx.getChild(2));
            TerminalNodeImpl op = (TerminalNodeImpl) ctx.getChild(1);
            int optype = op.getSymbol().getType();
            if (optype == gr02Lexer.STAR) {
                return v1 * v2;
            } else {
                return v1 / v2;
            }
        } else {
            return visit(ctx.getChild(0));
        }
    }

    @Override
    public Double visitPower(gr02Parser.PowerContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitPower\n");
        }
        if (ctx.children.size() == 3) {
            Double v1 = visit(ctx.getChild(0));
            Double v2 = visit(ctx.getChild(2));
            return Math.pow(v1, v2);
        } else {
            return visit(ctx.getChild(0));
        }
    }

    @Override
    public Double visitUnary(gr02Parser.UnaryContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitUnary\n");
        }
        ParseTree c0 = ctx.getChild(0);
        if (c0 instanceof TerminalNodeImpl) {
            TerminalNodeImpl op = (TerminalNodeImpl) c0;
            int optype = op.getSymbol().getType();
            if (optype == gr02Lexer.PLUS) {
                return visit(ctx.unary());
            } else if (optype == gr02Lexer.MINUS) {
                return -visit(ctx.unary());
            }
        }
        return visit(ctx.atom());
    }

    @Override
    public Double visitAtom(gr02Parser.AtomContext ctx) {
        if (verbose > 0) {
            System.out.printf("__: visitAtom\n");
        }
        ParseTree c0 = ctx.getChild(0);
        if (c0 instanceof TerminalNodeImpl) {
            TerminalNodeImpl op = (TerminalNodeImpl) c0;
            int optype = op.getSymbol().getType();
            if (optype == gr02Lexer.LPAREN) {
                return visit(ctx.expression());
            }
        }
        return Double.valueOf(ctx.NUMBER().getText());
    }
}
