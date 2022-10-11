class CodePrinter:
    def __init__(self, indent):
        self.indent = indent
        self.lines = []
        self.line_parts = []

    def enter_indent(self):
        self.indent += 2

    def leave_indent(self):
        self.indent -= 2

    def write(self, s):
        self.line_parts.append(s)

    def write_line(self, s: str):
        self.write(s)
        self.break_line()

    def write_and_enter_indent(self, s: str):
        self.write(s)
        self.break_line()
        self.enter_indent()

    def leave_indent_and_write(self, s: str):
        self.leave_indent()
        self.write(s)
        self.break_line()

    def break_line(self):
        line = (' ' * self.indent) + ''.join(self.line_parts)
        self.line_parts = []
        self.lines.append(line)

    def get_result(self):
        return "\n".join(self.lines)
