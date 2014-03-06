function CreateClass(base)
    local cls = {}
    cls.__index = cls
    
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
        end
    })
    
    return cls
end
