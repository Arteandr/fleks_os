local dap = require("dap")

dap.adapters.codelldb = {
  type = "server",
  port = "${port}",
  executable = {
	command = "/home/hwndrer/.local/share/nvim/mason/bin/codelldb",
	args = { "--port", "${port}" },
  },
}

dap.adapters.cppdbg = {
	id = "cppdbg",
	type = "executable",
	command = "/home/hwndrer/.local/share/nvim/mason/bin/OpenDebugAD7",
}

dap.configurations.cpp = {
	{
		name = "Launch file",
		type = "codelldb",
		request = "launch",
		program = function()
		    os.execute("make")
		    return vim.fn.getcwd() .. "/main" 
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
