import gdb

class ShowVars(gdb.Command):
    """showvars — print args, locals, statics, and globals for the current frame."""

    def __init__(self):
        super().__init__("showvars", gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):
        frame = gdb.selected_frame()
        if frame is None:
            print("showvars: no selected frame")
            return

        seen = set()

        def print_sym(sym, where):
            name = sym.print_name or sym.name
            if not name or name in seen:
                return
            try:
                val = sym.value(frame)
                print(f"{where:8} {name}: {val}")
            except gdb.error:
                # optimized out / unavailable
                print(f"{where:8} {name}: <unavailable>")
            seen.add(name)

        # Walk lexical blocks up to the function scope
        b = frame.block()
        while b is not None:
            for sym in b:
                if not sym.is_variable:
                    continue
                where = "arg" if getattr(sym, "is_argument", False) else ("local" if b.function else "block")
                print_sym(sym, where)
            if b.function:
                break
            b = b.superblock

        # Include file-scope statics and globals from the current symtab
        try:
            sal = frame.find_sal()
            st = getattr(sal, "symtab", None)
            if st:
                for blk, where in (
                    (getattr(st, "static_block", None), "static"),
                    (getattr(st, "global_block", None), "global"),
                ):
                    if blk:
                        for sym in blk:
                            if sym.is_variable:
                                print_sym(sym, where)
        except Exception:
            # Missing/partial symtab info; ignore quietly
            pass

ShowVars()
