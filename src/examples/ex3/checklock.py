import gdb

class CheckLock(gdb.Breakpoint):
    def __init__(self, var_address, lock_address):
        super().__init__(f"{var_address}", gdb.BP_WATCHPOINT, internal=False)
        self.var_address = var_address
        self.lock_address = lock_address

    def stop(self):
        value = gdb.parse_and_eval(f"{self.var_address}")
        lock = gdb.parse_and_eval(f"{self.lock_address}._M_mutex.__data.__lock")
        owner = gdb.parse_and_eval(f"{self.lock_address}._M_mutex.__data.__owner")
        current_thread = gdb.selected_thread().ptid[1]
        print(f"CheckLock: value={value} lock={lock} owner={owner} ct={current_thread}")
        if not lock :
            print(f"UNLOCKED ACCESS")
            return True
        if owner != current_thread:
            print(f"LOCKED ACCESS - wrong TID got {owner} expected {current_thread}")
            return True

        return False

class WatchVariableCommand(gdb.Command):
    """Command to watch a specific instance variable dynamically in GDB.
       Usage: watch_instance <address>
    """

    def __init__(self):
        super().__init__("check_lock", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        args = arg.split()
        if len(args) != 2:
            print("Usage: watch_instance <memory_address> <variable_name>")
            return

        memory_address, variable_name = args
        CheckLock(memory_address, variable_name)

WatchVariableCommand()
print("Custom GDB command 'check_lock' loaded.")