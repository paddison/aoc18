const std = @import("std");

pub fn solve() !void {
    // stdout is for the actual output of your application, for example if you
    // are implementing gzip, then only the compressed bytes should be sent to
    // stdout, not any debugging messages.
    const stdout_file = std.io.getStdOut().writer();
    var bw = std.io.bufferedWriter(stdout_file);
    const stdout = bw.writer();

    // open a file and allocate file size space to read it
    const allocator = std.heap.page_allocator;
    _ = allocator;
    const file = @embedFile(".\\input\\01.txt");

    // split the file at new lines
    var splits = std.mem.split(u8, file, "\r\n");
    var part_one = solve_part_one(&splits);
    splits.reset();
    try stdout.print("Part 1: {}\n", .{part_one});
    var part_two = try solve_part_two(&splits);
    try stdout.print("Part 2: {}", .{part_two});
    try bw.flush(); // don't forget to flush!
}

fn parseToNumber(split: []const u8) error{ InvalidCharacter, Overflow }!i32 {
    if (split.len < 2) return error.InvalidCharacter;

    var sign: i32 = if (split[0] == '+') 1 else -1;
    var num = try std.fmt.parseInt(i32, split[1..], 10);
    return sign * num;
}

fn solve_part_one(splits: *std.mem.SplitIterator(u8)) i32 {
    var position: i32 = 0;
    while (splits.next()) |split| {
        position += parseToNumber(split) catch continue;
    }
    return position;
}

fn solve_part_two(splits: *std.mem.SplitIterator(u8)) error{OutOfMemory}!i32 {
    var allocator = std.heap.page_allocator;
    var map = std.AutoHashMap(i32, void).init(allocator);
    defer map.deinit();
    var position: i32 = 0;
    try map.put(position, {});

    while (true) {
        var maybe_split = splits.next();
        var split: []const u8 = undefined;

        if (maybe_split == null) {
            splits.reset();
            split = splits.next().?;
        } else {
            split = maybe_split.?;
        }

        position += parseToNumber(split) catch continue;
        if (map.contains(position)) {
            return position;
        }
        try map.put(position, {});
    }

    return 0;
}
