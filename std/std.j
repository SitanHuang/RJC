/Jasm standard library

$int true 1
$int false 0

/global function puts
fcn puts string str
	#str + "\n"
end

fcn puts
	puts ""
end

load "std/stdstring.j"
load "std/stdarray.j"