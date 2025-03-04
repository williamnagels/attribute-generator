import gdb

class WatchInstanceVariable(gdb.Breakpoint):
    def __init__(self, var_address):
        super().__init__(f"{var_address}", gdb.BP_WATCHPOINT, internal=False)
        self.var_address = var_address

    def stop(self):
        new_value = gdb.parse_and_eval(f"{self.var_address}")
        thread_info = gdb.execute("info threads", to_string=True)

        thread_apply_all_bt = gdb.execute("thread apply all bt", to_string=True)
        with open(f"/tmp/gdb/gdb_{self.var_address}_{new_value}", "a") as f:
            f.write(f"{thread_info}\n")
            f.write(f"{thread_apply_all_bt}\n")
        return False

class WatchVariableCommand(gdb.Command):
    """Command to watch a specific instance variable dynamically in GDB.
       Usage: watch_instance <address>
    """

    def __init__(self):
        super().__init__("watch_instance", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        if not arg:
            print("Usage: watch_instance <memory_address>")
            return

        # Create a new watchpoint for the given address
        WatchInstanceVariable(arg)

WatchVariableCommand()
print("Custom GDB command 'watch_instance' loaded.")