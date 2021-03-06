let obj: {
	// overloadもできるよ
	(word: string): string;
	(): number;
};
// 当てはまる値はこんな感じ
// すべての引数と返り値に矛盾しないようにしなければならない…
obj = (word?: string): any => {
	if (typeof word === "string") {
		return `Hello, ${word}`;
	} else {
		return 42;
	}
};
// 呼び出してみよう！
let str = obj("TypeScript");
// Hello, TypeScript と表示される
console.log(str);

let num = obj();
// 42 と表示される
console.log(num);

export { }
