
/**
 * @author Anc
 * @version 3.7
 * 
 * features:
 * -standart arithmetics (add,sub,mul,div)
 * -decimal support
 * -nested terms in brackets
 * -appropriate omission of the '*' operator possible
 * -custom functions and constants
 */
public class Interpreter
{
    boolean debug = false;

    private Stack digits;  /** holds digits to be joined to one number */
    private int nest_level = 0;  /** has to be 0 again after parsing, else the error flag is set */
    private boolean frac_flag = false; /** (internal) indicates following digits will be processed as fractals */
    private byte error_type = 0; 
    private byte frac_point = 0;
    
    public Interpreter()
    {
        digits = new Stack();
    }
    
    public double parse(String arg)
    {
        nest_level = 0;
        error_type = 0;
        return _parse(arg);
    }
        
    
    private double _parse(String arg)
    {
        Stack arguments = new Stack();  /** holds float values */
        Stack operators = new Stack();  /** holds arithmetic operators */
       
        int i = 0;
        int local_nest_level = 0;
        byte last_p = 0;  /** priority of last parsed operator */
        boolean coef_flag = false;  /** set if value might preceed a bracket and thus become a coefficient */
        boolean func_flag = false;  /** set if result of next bracket is to be passed as an argument to function <symbol> */
        boolean nest_flag = false;  /** indicates characters are being collected and not parsed */
        boolean nest_init_flag = false;
        boolean neg_flag = true;  /** set if next '-' will make number signed and not be an operator */
        String nested_term = ""; /** collector string for contents of nested term */
        String symbol = ""; /** collector string for symbol name */
        frac_flag = false;
        char[] args = arg.toCharArray(); /** map out String to pure char array */
        for (i=0; i < arg.length(); i++)
        {
            
            if (nest_init_flag) {nest_init_flag = false;}
            
            /** lock computing by raising nest level, substitute '*' if coefficient exists */
            if (args[i] == '(')
            {
                if (!nest_flag) /** nested interpreting is just about to be initialized */
                {
					if (coef_flag) {operators.push('*'); last_p = priority('*');}
					coef_flag = true;
                    nest_flag = true;
                    nest_init_flag = true;
                    nest_level += 1;
                }
                else  /** nested interpreting is in progress */
                {
                    local_nest_level += 1;
                }
            }
            
            else if (args[i] == ')')
            {
                if (nest_flag && local_nest_level == 0) /** nesting has reached end */
                {
                    nest_flag = false; nest_level -= 1;
                    double nested_term_result = _parse(nested_term);
                    nested_term = "";
                    if (func_flag)
                    {
                        arguments.push(compute_function(nested_term_result, symbol));
                        func_flag = false;
                        symbol = "";
                    }
                    else {arguments.push(nested_term_result);}
                }
                else  /** nested interpreting is in progress */
                {
                    local_nest_level -= 1;
                }
            }
            
            
            if (!nest_flag)
            {
                
                if (args[i] == '.' || args[i] == ',')
                {
                    if (isNumber(char_at(arg,i+1))) {frac_flag = true;}
                    else {error_type = 3; return 0;}
                }
            
                else if (isNumber(args[i]))  /** parse number */
                {
					digits.push(to_i(args[i]));
                    if (frac_flag) {frac_point -= 1;}
                    /** check if there is more than one digit or fractal part */
                    if (!(isNumber(char_at(arg, i+1)) || char_at(arg, i+1) == '.' || char_at(arg, i+1) == ','))
                    {
                        if (coef_flag) {operators.push('*'); last_p = priority('*');}
                        arguments.push(join_digits(frac_point));
                        neg_flag = false; coef_flag = true; frac_flag = false; frac_point = 0;
                    }
                }
            
                else if (isOperator(args[i]))  /** parse operators */
                {
                    if (neg_flag && args[i] == '-')  /** check if preceeding minus changes sign of next symbol */
                    {
                        neg_flag = false;
                        arguments.push(-1.0); operators.push('*'); last_p = priority('*');
                    }
                    else
                    {
						if (arguments.length() <= operators.length()) {error_type = 4; break;}
                        /** check beforehand if lower priority operator is encountered */
                        if (priority(args[i]) < last_p) {if (nest_level == 0) {compute(arguments, operators);}}
                        last_p = priority(args[i]);
                        operators.push(args[i]);
                        coef_flag = false;
                        neg_flag = true;
                    }
                }
                
                else if (isLetter(args[i])) /** parse letters */
                {
					if (coef_flag) {coef_flag = false; operators.push('*'); last_p = priority('*');}
					if (neg_flag) {neg_flag = false;}
					symbol = symbol.concat(to_s(args[i]));
                    if (char_at(arg,i+1) == '(')
                    {
						compute_function(0, symbol);
						if (error_type != 0)
						{
							error_type = 0;
							arguments.push(lookup_constant(symbol));
							symbol = "";
							coef_flag = true;
						}
						else {func_flag = true;}
					}
					else if (!isLetter(char_at(arg,i+1)))
					{
						arguments.push(lookup_constant(symbol));
						symbol = "";
						coef_flag = true;
					}
				}
            
            }
            
            else if (!nest_init_flag) /** this collector block needs to be skipped once so the first '(' isn't collected */
            {
                Character ch = args[i];
                nested_term = nested_term.concat(ch.toString());
            }
            
            if (args[i] == ' ') {coef_flag = false;}
            
            if (char_at(arg,i) == '#') {break;}  /** failsafe, in case array bounds are left */
        }
        // dump_stacks();
        if ((nest_level != 0 || local_nest_level != 0) && error_type == 0) {error_type = 1;}
        if (neg_flag && error_type == 0) {error_type = 4;}
        if (error_type == 0) {compute(arguments, operators);}
        if (arguments.length() > 1 && error_type == 0) {error_type = 4;}
        if (error_type == 0) {return (Double)arguments.top();}
        else {return 0;}
    }
    
    /** 0 => no errors occured;  1 => bracket error;  2 => unknown symbol;
     *  3 => no digits after point; 4 => operator error */
    public byte error_type() {return error_type;}
    
    
    /** array wrapper */
    private char char_at(String array, int i)
    {
        if (i >= 0 && i <= array.length()-1)
        {
            return array.charAt(i);
        }
        else {return '#';}
    }
    
    /** check if char represents a number */
    private boolean isNumber(char c)
    {
        int j = (int) c;
        return (j >= 48 && j <= 57);
    }
    
    /** check if char represents an operator */
    private boolean isOperator(char c)
    {
        int j = (int) c;
        return ((j >= 42 && j <= 47 && j != 44 && j != 46) || j == 94);
    }
    
   private boolean isLetter(char c)
   {
       int j = (int) c;
       return ((j >= 65 && j <= 90) || (j >= 97 && j <= 122));
   }
    
    /** determin priority from operator */
    private byte priority(char c)
    {
             if (c == '+' || c == '-') {return 0;}
        else if (c == '*' || c == '/') {return 1;}
        else if (c == '^') {return 2;}
        else {return 7;}
    }
    
    /** compute until operator stack is empty (result is stored in arguments stack) */
    private void compute(Stack arguments, Stack operators)
    {
        if (debug) {print("Computing!");}
        if (arguments.length()-1 != operators.length() && error_type == 0) {error_type = 4;}
        while (!operators.isEmpty())
        {
            Double b_ = (Double)arguments.top(); arguments.pop(); double b = b_.doubleValue();
            Double a_ = (Double)arguments.top(); arguments.pop(); double a = a_.doubleValue();
            Character op_ = (Character)operators.top(); operators.pop(); char op = op_.charValue();
            arguments.push(calculate(a, b, op));
        }
    }
    
    private double compute_function(double argument, String f_name)
    {
        if (debug) {print(f_name);}
        
             if (f_name.equals("foo"))       {return argument;}
        else if (f_name.equals("square"))    {return argument*argument;}
        
        else {error_type = 2; return 0;}
        
        /**switch (f_name)
        {
            case "foo"     : return argument;
            case "square"  : return argument*argument;
            default        : error_flag = true; error_type = 2; return 0;
        }*/
    }
    
    private double lookup_constant(String const_name)
    {
		     if (const_name.equals("four"))   {return 4;}
		else if (const_name.equals("pi"))     {return Math.PI;}
		
		else {error_type = 2; return 0;}
	}
    
    /** carry out primitive arithmetics */
    private double calculate(double a, double b, char op)
    {
        if (debug) {print("Calculating!");}
        switch (op)
        {
            case '+': return a+b;
            case '-': return a-b;
            case '*': return a*b;
            case '/': return a/b;
            case '^': return Math.pow(a,b);
            default: return 0;
        }
    }
    
    private double power(double b, int e)
    {
        double ret;
        if (e >= 0)
        {
            int i;
            ret = 1;
            for (i=0;i<e;i++) {ret *= b;}
        }
        else
        {
            double denom = power(b, -1*e);
            ret = 1/denom;
        }
        return ret;
    }
    
    /** convert char to int */
    private double to_i(char c)
    {
        double j = (double) c;
        return j-48;
    }
    
    private String to_s(char c)
    {
        Character ch = c;
        return ch.toString();
    }
    
    /** join digits to one int */
    private double join_digits(int frac_point)
    {
        int i;
        double ret = 0;
        double temp2;
        int max = digits.length();
        for (i=frac_point;i<max+frac_point;i++)
        {
            Double temp = (Double)digits.top(); digits.pop();
            temp2 = temp.doubleValue();
            temp2 *= power(10,i);
            ret += temp2;
        }
        return ret;
    }
    
    
    /** clear stacks */
    private void empty_stacks(Stack arguments, Stack operators)
    {
        while (!arguments.isEmpty()) {arguments.pop();}
        while (!operators.isEmpty()) {operators.pop();}
        while (!digits.isEmpty()) {digits.pop();}
    }
    
    /** debugging */
    
/**    public void dump_stacks(Stack arguments, Stack operators)
    {
        if (debug) {
        print("<Op>");
        while (!operators.isEmpty())
        {
            System.out.println(operators.top());
            operators.pop();
        }
        print("<Arg>");
        while (!arguments.isEmpty())
        {
            System.out.println(arguments.top());
            arguments.pop();
        } }
    }*/
    
    //public void push_digits(double i) {digits.push(i);}
    
    private void print(String s) {System.out.println(s);}

}
