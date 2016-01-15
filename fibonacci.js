function fib(a){
    if (a == 1) return 1;
    if (a == 2) return 1;
    return fib(a-1) + fib(a-2);
}

var gol = 10;

function output(){
    return gol;
}

fib(10);

function f1(){
    var n=999;
    nAdd=function(){n+=1}


    function f2(){
    　　alert(n);
    }
    return f2;
}

var result=f1();
result(); // 999
nAdd();
result(); // 1000
