# RJC Programming language
written in c++, by the java programmer
kdevelop 4 project (cmake)
c++14
LICENSE: !()[http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-1.png]
```
load "std/std.j"

:int test 5
if test bg 4
	puts 'test bg 4'
end
if test sm 5
	puts 'test sm 5'
elif test sm 6
	puts 'test sm 6'
end
if (test sm 5) or (test bg 4)
	puts 'hahaha'
end

puts "
--------------\n
"

:class a array
puts a
a.set 0,'1'
a.set 1,'2'
a.set 2,'['jfffdsjfdsjflkdsflkds',1,5]'
a.setString 3,'hahahaha'
puts a
puts a.size;
puts a.at 2;

```

output:
```
test bg 4
test sm 6
hahaha

--------------


[]
[1,2,[" jfffdsjfdsjflkdsflkds",1,5],"hahahaha"]
4
[" jfffdsjfdsjflkdsflkds",1,5]
```
