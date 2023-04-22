const std = @import("std");

const file = @embedFile("data.txt");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    var string_it = std.mem.tokenize(u8, file, "\r\n");

    const strings = try get_strings(gpa.allocator(), &string_it);
    defer strings.deinit();

    var part_one = try solve_part_one(gpa.allocator(), strings);
    std.debug.print("Part 1: {}\n", .{part_one});
    var part_two = try solve_part_two(gpa.allocator(), strings);
    std.debug.print("Part 2: {s}\n", .{part_two.items});
}

fn get_strings(alloc: std.mem.Allocator, string_it: *std.mem.TokenIterator(u8)) !std.ArrayList([]const u8) {
    var strings = std.ArrayList([]const u8).init(alloc);
    errdefer strings.deinit();
    while (string_it.next()) |string| {
        try strings.append(string);
    }
    return strings;
}

fn solve_part_one(allocator: std.mem.Allocator, strings: std.ArrayList([]const u8)) !u32 {
    var twice: u32 = 0;
    var thrice: u32 = 0;
    for (strings.items) |string| {
        const tpl = try contains_twice_thrice(allocator, string);
        twice += tpl[0];
        thrice += tpl[1];
    }
    return twice * thrice;
}

fn solve_part_two(allocator: std.mem.Allocator, strings: std.ArrayList([]const u8)) !std.ArrayList(u8) {
    var result_string = std.ArrayList(u8).init(allocator);

    for (strings.items, 0..) |lhs, index| {
        if (result_string.items.len > 0) {
            break;
        }
        for (strings.items[index + 1 ..]) |rhs| {
            const cmp_result = (count_non_equals(lhs, rhs) catch continue) orelse continue;
            for (lhs, 0..) |char_lhs, j| {
                if (j != cmp_result) {
                    try result_string.append(char_lhs);
                }
            }
            break;
        }
    }
    return result_string;
}

fn contains_twice_thrice(allocator: std.mem.Allocator, string: []const u8) ![2]u8 {
    var tpl = [2]u8{ 0, 0 };
    var seen = std.AutoHashMap(u8, void).init(allocator);
    for (string, 0..) |lhs_char, index| {
        var count: u8 = 1;
        if (seen.contains(lhs_char)) continue;
        try seen.put(lhs_char, {});
        for (string[index + 1 ..]) |rhs_char| {
            if (lhs_char == rhs_char) count += 1;
        }
        if (count == 2) {
            tpl[0] = 1;
        }
        if (count == 3) tpl[1] = 1;
    }

    return tpl;
}

fn count_non_equals(lhs: []const u8, rhs: []const u8) !?usize {
    const allocator = std.heap.page_allocator;
    var index_list = std.ArrayList(usize).init(allocator);
    for (lhs, 0..) |_, index| {
        if (lhs[index] != rhs[index]) {
            try index_list.append(index);
        }
    }
    if (index_list.items.len == 1) {
        return index_list.items[0];
    }
    return null;
}
