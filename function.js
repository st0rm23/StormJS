function fib(a){
    var n = 1;

    function sb(){
        n = n+1;
        output(n);
    }
    return sb;
    //return fib(a-1) + fib(a-2)
}

var func = fib(3);
func();
func();

var ff = fib(4);
ff();
