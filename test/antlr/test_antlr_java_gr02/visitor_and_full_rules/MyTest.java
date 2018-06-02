import java.io.InputStream;
import java.io.ByteArrayInputStream;
import java.nio.charset.StandardCharsets;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

public class MyTest {
    int m_verbose = 0;

    void testExpression(String expr_text, double expected) throws Exception {
        if (m_verbose > 0) {
            System.out.printf("testExpression(<%s>)\n", expr_text);
        }
        try {
            InputStream string_reading = new ByteArrayInputStream(expr_text.getBytes(StandardCharsets.UTF_8));
            ANTLRInputStream input = new ANTLRInputStream(string_reading);
            gr02Lexer lexer = new gr02Lexer(input);
            CommonTokenStream tokens = new CommonTokenStream(lexer);
            gr02Parser parser = new gr02Parser(tokens);
            ParseTree tree = parser.expression();
            MyVisitor visitor = new MyVisitor();
            visitor.verbose = m_verbose;
            Double result = visitor.visit(tree);
            if (m_verbose > 0) {
                System.out.println("Result: " + result);
            }
            if (result != expected) {
                String et = String.format("Test failed: expr=<%s> expected=%s result=%s", expr_text, expected, result);
                throw new RuntimeException(et);
            }
        } catch (RuntimeException exc) {
            String msg = String.format("Test failed: expr=<%s> exception=<%s>", expr_text, exc);
            System.out.println(msg);
            throw exc;
        }
    }

    void testExpressionFail(String expr_text) throws Exception {
        if (m_verbose > 0) {
            System.out.printf("testExpressionFail(<%s>)\n", expr_text);
        }
        boolean failed = false;
        try {
            InputStream string_reading = new ByteArrayInputStream(expr_text.getBytes(StandardCharsets.UTF_8));
            ANTLRInputStream input = new ANTLRInputStream(string_reading);
            gr02Lexer lexer = new gr02Lexer(input);
            CommonTokenStream tokens = new CommonTokenStream(lexer);
            gr02Parser parser = new gr02Parser(tokens);
            if (m_verbose <= 0) {
                parser.setErrorHandler(new BailErrorStrategy());
                lexer.removeErrorListener(ConsoleErrorListener.INSTANCE);
                parser.removeErrorListener(ConsoleErrorListener.INSTANCE);
            }
            ParseTree tree = parser.expression();
            MyVisitor visitor = new MyVisitor();
            visitor.verbose = m_verbose;
            Double result = visitor.visit(tree);
            if (m_verbose > 0) {
                System.out.println("Result: " + result);
            }
            failed = (result == null);
        } catch (RuntimeException exc) {
            // FIXME: narrow the exception set?
            failed = true;
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
        testExpressionFail("-((2+3)");
        //- testExpressionFail("-(2+3))");
        //- testExpressionFail("2* *2");
    }

    public static void main(String[] _args) throws Exception {
        MyTest prog = new MyTest();
        prog.run();
    }
}
