import gdb

class WatchWindow:
    def __init__(self, tui_window):
        self._tui_window = tui_window
        self._tui_window.title = 'Watching'
        self.variables = []

    def watch(self, variable):
        self.variables.append(variable)
        self.update()

    def update(self):
        self.render()

    def render(self):
        self._tui_window.erase()
        for variable in self.variables:
            value = gdb.parse_and_eval(f"{variable}")
            self._tui_window.write(f'{variable}={value}\n')


g_watch_window = None
def create_watch_window(tui_window):
    global g_watch_window
    g_watch_window = WatchWindow(tui_window)

gdb.register_window_type('WatchWindow', create_watch_window)

class WatchBreakpoint(gdb.Breakpoint):
    def __init__(self, name):
        super().__init__(f"{name}", gdb.BP_WATCHPOINT, internal=False)
        self.name = name
        g_watch_window.watch(self.name)

    def stop(self):
        global g_watch_window
        if g_watch_window == None:
            return False
        g_watch_window.update()
        return False


class WatchWindowCommand(gdb.Command):
    """Command to watch a specific instance variable dynamically in GDB.
       Usage: watch_instance <address>
    """

    def __init__(self):
        super().__init__("swe_watch", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        args = arg.split()
        if len(args) != 1:
            print("Usage: swe_watch <variable name>")
            return


        name = args[0]
        print(f"Started watching {name}")
        WatchBreakpoint(name)

WatchWindowCommand()
print("Custom GDB command 'swe_watch' loaded.")