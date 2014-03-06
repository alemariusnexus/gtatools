function CreateClass(base)
    local cls = {}
    cls.__index = cls
    
    if base then
    	cls._basetable = {base = true}
    else
    	cls._basetable = {}
    end
    
    if base then
        for name, method in pairs(base) do
            cls[name] = method
        end
    end
    
    setmetatable(cls, {
        __call = function(c, ...)
            local inst = setmetatable({}, c)
            local init = inst._init
            
            if init then
                init(inst, ...)
            end
            
            return inst
        end
    })
    
    
    function cls:instanceof(othercls)
    	local cls = getmetatable(self)
    	return othercls == cls  or  cls._basetable[othercls] ~= nil
    end
	
    
    return cls
end



