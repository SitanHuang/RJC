# RJC Programming language
written in c++, by the java programmer

kdevelop 4 project (cmake)

c++14

LICENSE: ![](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-1.png)

Garbage Collector is **WIP**

### Example 1

```ruby
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

### Operator overloading

```
load 'std/std.j'

class num
	fcn init int i
		@int v i
	end

	fcn '+' int i
		v v - i
	end

	fcn toString
		return string v
	end

end

:class n num 5
n."+" 4
puts n
```

output:
```
1
```

### Example 2
```ruby
load "std/std.j"

class printer

	puts '-----'

	fcn init string s
		@string value s
	end

	fcn init
		printer$init nil
	end

	fcn print int times
		:int temp 0
		loop temp neq times
			puts value
			temp temp + 1
		end
	end

	fcn toString
		return string value
	end

	fcn setValue string s
		value s
	end
	
end

:class p printer
puts p
p.setValue "Hello world!"
puts p
p.print 5
```
output:
```
-----
$nil$
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
```
