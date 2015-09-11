/Jasm Standard String Library
$string nil "$nil$"

class string
	fcn init string value
		@string value value
	end
	
	fcn toArray
		callnative string$toarray
	end
	
	fcn init
		string$init ""
	end
	
	fcn println
		puts value
	end
	
	fcn toString
		return string value
	end
	
end

