import java.io.InputStream;
import java.io.ByteArrayInputStream;
import java.nio.charset.StandardCharsets;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

public class MyTest {
    int m_verbose = 0;

    void testBlock(String test_name, String[] expr_block, double expected) throws Exception {
        if (m_verbose > 0) {
            System.out.printf("testExpression(<%s>)\n", test_name);
        }
        try {
            ExecutionContext econtext = new ExecutionContext();
            Double result = Double.NaN;
            for (String expr_line : expr_block) {
                InputStream string_reading = new ByteArrayInputStream(expr_line.getBytes(StandardCharsets.UTF_8));
                ANTLRInputStream input = new ANTLRInputStream(string_reading);
                gr04Lexer lexer = new gr04Lexer(input);
                CommonTokenStream tokens = new CommonTokenStream(lexer);
                gr04Parser parser = new gr04Parser(tokens);
                ParseTree tree = parser.statement();
                MyVisitor visitor = new MyVisitor();
                visitor.verbose = m_verbose;
                // NB visit() has no extra arguments
                visitor.econtext = econtext;
                SemanticValue rsv = visitor.visit(tree);
                if (rsv != null) {
                    result = rsv.getRValue(econtext);
                }
            }
            if (m_verbose > 0) {
                System.out.println("Result: " + result);
            }
            if (result != expected) {
                String et = String.format(
                        "Test failed: expr=<%s> expected=%s result=%s",
                        test_name, expected, result);
                throw new RuntimeException(et);
            }
        } catch (RuntimeException exc) {
            String msg = String.format("Test failed: expr=<%s> exception=<%s>", test_name, exc);
            System.out.println(msg);
            throw exc;
        }
    }

    void testExpression(String test_expr, double expected) throws Exception {
        testBlock(test_expr, new String[]{test_expr}, expected);
    }

    void testExpressionFail(String expr_text) throws Exception {
        if (m_verbose > 0) {
            System.out.printf("testExpressionFail(<%s>)\n", expr_text);
        }
        boolean failed = false;
        try {
            InputStream string_reading = new ByteArrayInputStream(expr_text.getBytes(StandardCharsets.UTF_8));
            ANTLRInputStream input = new ANTLRInputStream(string_reading);
            gr04Lexer lexer = new gr04Lexer(input);
            CommonTokenStream tokens = new CommonTokenStream(lexer);
            gr04Parser parser = new gr04Parser(tokens);
            if (m_verbose <= 0) {
                parser.setErrorHandler(new BailErrorStrategy());
                lexer.removeErrorListener(ConsoleErrorListener.INSTANCE);
                parser.removeErrorListener(ConsoleErrorListener.INSTANCE);
            }
            // What to call in parser is the starting rule.
            ParseTree tree = parser.statement();
            MyVisitor visitor = new MyVisitor();
            visitor.verbose = m_verbose;
            ExecutionContext econtext = new ExecutionContext();
            // NB visit() has no extra arguments
            visitor.econtext = econtext;
            Double result = visitor.visit(tree).getRValue(econtext);
            if (m_verbose > 0) {
                System.out.println("Result: " + result);
            }
            //failed = (result == null);
        } catch (RuntimeException exc) {
            // FIXME: narrow the exception set?
            failed = true;
            if (m_verbose > 0) {
                System.out.printf("testExpressionFail: caught an expected error: %s\n", exc);
            }
        }
        if (!failed) {
            String msg = String.format("testExpressionFail no exception: expr=<%s>", expr_text);
            throw new RuntimeException(msg);
        }
    }

    void run() throws Exception {
        String vstr = System.getenv("VERBOSE");
        if (vstr != null) {
            m_verbose = Integer.valueOf(vstr);
        }
        testExpression("0", 0);
        testExpression("+0", 0);
        testExpression("-0", 0);
        testExpression("1", 1);
        testExpression("+2", 2);
        testExpression("-3", -3);
        testExpression("-1", -1);
        testExpression("--1", 1);
        testExpression("++1", 1);
        testExpression("+-1", -1);
        testExpression("1+1", 2);
        testExpression("1+-1", 0);
        testExpression("3+4*5", 23);
        testExpression("3+(4*5)", 23);
        testExpression("(3+4)*5", 35);
        testExpression("-(3+4)*5", -35);
        testExpression("(3 + 4) * 5", 35);
        testExpression("(2+3) * (7-4) + 11", 26);
        testExpression("(2+3) * -(7-4) + 11", -4);
        testExpression("2**2", 4);
        testExpression("-2**2", 4);
        testExpression("2**-2", 0.25);
        testExpression("3*3**3", 81);
        testExpression("2**2**2", 16);
        testExpression("2**3**2", 512);
        testExpressionFail("+");
        testExpressionFail("");
        testExpressionFail("(");
        testExpressionFail(")");
        testExpressionFail("2+");
        //- testExpressionFail("$");
        //- testExpressionFail("2$");
        //- testExpressionFail("-((2+3)");
        //- testExpressionFail("-(2+3))");
        //- testExpressionFail("2* *2");

        // Test set - variables assignments and their usage
        testBlock("block1", new String[]{"a=1", "a"}, 1);
        testBlock("block2", new String[]{"a=3", "b=5", "a+b"}, 8);
        testBlock("block3", new String[]{"a=b=5", "a*b"}, 25);
        testBlock("block4", new String[]{"a=44"}, 44);
        testBlock("block5", new String[]{"(a)=177"}, 177);
        testBlock("block6", new String[]{"@defmap zz", "zz[1]=50"}, 50);
        testBlock("block7", new String[]{"@defmap zxcv", "(zxcv[1])=255", "(zxcv[(3-2)]-2)"}, 253);
        testBlock("block8", new String[]{"@defmap yx", "yx[1]=50", "yx[2]=yx[3]=4",
                "mm=8", "yx[1]*mm + yx[2]/yx[3]"}, 401);
        //- testBlockFail(new String[]{"@hello"});
        //- testBlockFail(new String[]{"1[2]"});
        //- testBlockFail(new String[]{"(c=d)=3"});
    }

    public static void main(String[] _args) throws Exception {
        MyTest prog = new MyTest();
        prog.run();
    }
}
