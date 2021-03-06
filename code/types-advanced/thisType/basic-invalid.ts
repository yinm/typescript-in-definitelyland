// ThisType は特殊なマーカーとしての型
interface Hello {
    hello(): string;
}
interface Named {
    name: string;
}

function withHello<T extends Named>(obj: T & ThisType<T & Hello>): T & Hello {
    const result: T & Hello = obj as any;
    result.hello = function (this: T) {
        return `Hello, ${this.name}`;
    };
    return result;
}

// https://github.com/Microsoft/TypeScript/issues/17041
// これコンパイル通らないのおかしいでしょという気持ち
const v = withHello({
    name: "TypeScript",
    sayHello() {
        console.log(this.hello());
    },
});
v.sayHello();

export {}
