xs script 简要说明
（by 大大唐 Email 27261096@qq.com 微信公众号：ddyzyjy 大大游资网站 www.dadayouzi.com ）
（2019.08.09）

这是多年前我编写的一个脚本语言，使用POSIX C编写，所以可以移植到几乎所有的系统，LINUX, ANDROID, IOS, MACOS, SYMBIAN, WINDOWS等等。
该脚本语言是基于堆栈的（C#，Perl等语言是基于堆栈，而Lua等是基于寄存器，基于堆栈的易扩展，而基于寄存器的执行速度快）。
该脚本语言最大的特点是简单，以极少的代码，逻辑清晰的实现了一个完整的五脏俱全的脚本语言，并完美的应用于实际生产环境中（实际编写执行过大量复杂的数千行的动画脚本）。
本文档已包括全部的脚本源代码，并提供了一个test测试程序和test.xs,hellworld.xs测试脚本（实现曼特波罗分形算法和打印hellworld），如果有安装GCC，则可以直接用build.bat来重新编译本脚本语言测试程序。
可以直接自行编写脚本，然后在windows 命令行中输入test hellworld.xs来执行你的脚本。可以通过简单的io_print函数打印结果，看脚本执行情况。
在你的主程序中使用xs脚本，需要先new一个vm，然后compile脚本，然后run，最后free vm即可。可参见测试程序test.c。

主要包括几个模块：
词法分析器(Lexer)，生成关键字、运算符等。
语义解析器（Parser），生成语法树
语义检查器（Semantic Checker），语义检查，生成中间码
优化器（Optimizer），优化代码
执行代码生成器（Code Generator），生成执行代码
虚拟机（Virtual Machine），执行代码
垃圾回收（Garbage Collection），回收释放引用数已经为0的对象
扩展库（Library Support），支持自行加入各种扩展库，默认提供几个最基本的库：math, system, string, queue, io, binary(脚本对二进制数据的读写操作)。提供一个sys_run(script)函数可以在脚本中动态执行脚本。
调试器（Debug），支持断点调试，暂停、进入、跳过等，支持联网调试（比如从PC调试手机），提供源代码级调试，及二进制指令级调试

基本语法：
类似于c风格语法，支持for、while循环，支持if else elseif及switch case分支控制语句，continue、break等循环跳出关键字，支持数组[]，另外字符串连接用".."。

基本指令集：
OpCode	Name	Description
0	NOP	空指令，不执行任何操作
		
1	ADD	Addition operator，加法操作
2	SUB	Subtraction operator，减法操作
3	MUL	Multiplication operator，乘法操作
4	DIV	Division operator，除法操作
5	MOD	Modulus（remainder）operator，求余操作
6		
11	NOT	Logical NOT operator，逻辑求反操作
12	AND	Logical AND operator，逻辑与操作
13	OR	Logical OR Operator，逻辑或操作
14	XOR	Logical XOR Operator，逻辑异或操作
15		
21	EQ	Equality test，进行等于比较
22	LT	Less than test，进行小于比较
23	LE	Less than or equal to test，小于或等于
24	GT	Greater than test，进行大于比较
25	GE	Greater than or equal to，大于或等于
26		
31	JMP	Unconditional jump，无条件跳转
32	JMPC	Conditional jump，有条件跳转
33	CALL	Call function
34	RET	Return from function call，从函数返回
		
41	INT	Push an integer into stack，将一个整数压入堆栈
42	BOOL	Push a Boolean into stack，将一个布尔值压入堆栈
43	FLOAT	Push a float into stack，将一个双精度浮点数压入堆栈
44	STRING	Push a string into stack，将一个字串压入堆栈
45	PUSH	Push an object into stack，将变量压入堆栈
46	POP	Popup an object from stack，从堆栈弹出一个对象
		
51	CONC	Concatenate operator，连接两个字串
52	LEN	Length operator，取长度
53		
54		

运算符优先级
0级，+ -
1级，*  /  %
2级，- ！
3级，（）
自右向左运算

数据类型： 
空值n
变量v
布尔b
数字n
字串s
