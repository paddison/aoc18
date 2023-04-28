const std = @import("std");
const file = @embedFile("./data.txt");

const Ids = std.AutoArrayHashMap(u16, void);
const Grid = std.AutoArrayHashMap(struct { usize, usize }, [2]u16);

// const Claim = struct { x: u16, y: u16, w: u16, h: u16 };

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    var grid = Grid.init(gpa.allocator());
    defer grid.deinit();
    var overlapping = Ids.init(gpa.allocator());
    defer overlapping.deinit();
    var all = Ids.init(gpa.allocator());
    defer all.deinit();
    var data_it = std.mem.tokenize(u8, file, "#@,:x\n\r ");

    while (data_it.next()) |token| {
        const id = try tokenToU16(token);
        try all.put(id, {});
        var x = try tokenToU16(data_it.next().?);
        var y = try tokenToU16(data_it.next().?);
        var width = try tokenToU16(data_it.next().?);
        var height = try tokenToU16(data_it.next().?);

        for (x..x + width) |i| {
            for (y..y + height) |j| {
                const key = .{ i, j };
                if (!grid.contains(key)) {
                    try grid.put(key, [_]u16{ 1, id });
                } else {
                    var val = grid.get(key).?;
                    val[0] += 1;
                    try overlapping.put(id, {});
                    try overlapping.put(val[1], {});
                    try grid.put(key, val);
                }
            }
        }
    }

    var count: usize = 0;
    for (grid.values()) |val| {
        if (val[0] > 1) {
            count += 1;
        }
    }

    std.debug.print("Part 1: {}\n", .{count});

    for (all.keys()) |id| {
        if (!overlapping.contains(id)) {
            std.debug.print("Part 2: {}\n", .{id});
            break;
        }
    }
}

fn tokenToU16(token: []const u8) !u16 {
    return std.fmt.parseInt(u16, token, 10);
}
