local dap = require("dap")

dap.adapters.codelldb = {
  type = "server",
  port = "${port}",
  executable = {
	command = "/home/hwndrer/.local/share/nvim/mason/bin/codelldb",
	args = { "--port", "${port}" },
  },
}

dap.configurations.cpp = {
	{
		type = "codelldb",
		request = "launch",
		program = function()
		    -- Compile and return exec name
		    os.execute("make")
		    return vim.fn.getcwd() .. "/main" 
		end,
		args = function ()
			local argv = {}
			arg = vim.fn.input(string.format("argv: "))
			for a in string.gmatch(arg, "%S+") do
				table.insert(argv, a)
			end
			vim.cmd('echo ""')
			return argv
		end,
		cwd = "${workspaceFolder}",
		stopOnEntry = false,
		MIMode = "gdb",
		miDebuggerPath = "/usr/bin/gdb",
		setupCommands = {
			{
				text = "-enable-pretty-printing",
				description = "enable pretty printing",
				ignoreFailures = false,
			}
		}
	},
}

dap.configurations.c = dap.configurations.cpp
