var tuple: [string, number] = ["str", 1];

// 先頭を number に…
tuple.unshift(1);

// あぁっ！実行時エラー！
// Uncaught TypeError: undefined is not a function
tuple[0].charAt(0);
