--[[
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
]]

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



