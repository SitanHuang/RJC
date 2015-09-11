/Jasm Standard Array Library
class array
	fcn init string str
		@string value str
		callnative array$init value,false
	end
	
	fcn init
		array$init "[]"
	end
	
	fcn size
		callnative array$init value,true
		return int size
	end
	
	
	fcn set int index,string str
		callnative array$init value,true
		callnative array$set index,str
		callnative array$init value,true
	end
	
	fcn setString int index,string str
		str "\"\"" + str + "\"\""
		array$set index,str
	end
	
	fcn at int index
		callnative array$init value,true
		callnative array$at index
	end
	fcn toString
		return string value
	end
end
