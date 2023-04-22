const std = @import("std");
const p01 = @import("./01/01.zig");

pub fn main() !void {
    // stdout is for the actual output of your application, for example if you
    // are implementing gzip, then only the compressed bytes should be sent to
    // stdout, not any debugging messages.
    const stdout_file = std.io.getStdOut().writer();
    var bw = std.io.bufferedWriter(stdout_file);
    _ = bw.writer();
    try p01.solve();
    try bw.flush(); // don't forget to flush!
}
