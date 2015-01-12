
clang_complete = {}
clang_complete.generate = function(prj)
    local clang_complete = prj.basedir .. "/.clang_complete"

    os.remove(filename)

    local marked = {}
    local f = io.open(clang_complete, "w")

    if os.is64bit() then
        f:write("-m64\n")
    end

    for cfg in premake.project.eachconfig(prj) do
        if table.contains(cfg.terms, "release") then
            for _, includedir in ipairs(cfg.includedirs) do
                if not marked[includedir] then
                    f:write("-I" .. includedir .. "\n")
                    marked[includedir] = includedir
                end
            end

            for _, buildoption in ipairs(cfg.buildoptions) do
                if not marked[buildoption] then
                    f:write(buildoption .. "\n")
                    marked[buildoption] = buildoption
                end
            end

            for _, buildoption in ipairs(premake.tools.gcc.cxxflags.system[os.get()]) do
                if not marked[buildoption] then
                    f:write(buildoption .. "\n")
                    marked[buildoption] = buildoption
                end
            end

            for _, define in ipairs(cfg.defines) do
                if not marked[define] then
                    f:write("-D" .. define .. "\n")
                    marked[define] = define
                end
            end
        end
    end

    f:close()
end

newaction
{
    trigger         = "clang-complete",
    shortname       = "Clang Complete",
    description     = "Generate the include files used by Clang autocomplete plugins.",

    valid_languages = { "C", "C++" },

    valid_tools     =
    {
        cc     = { "clang"},
    },

    valid_platforms =
    {
        linux32		= "linux32",
        linux64     = "linux64"
    },

    oncleanproject = function(prj)
        os.remove(prj.basedir .. "/.clang_complete")
    end,

    onproject = function(prj)
        print("Generating .clang_complete for project " .. prj.name .. "...")
        clang_complete.generate(prj)
    end
}
