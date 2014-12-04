= JS資産と型定義ファイル

== JavaScriptの資産が使いたい！

TypeScriptはJavaScriptの上位互換であり、JavaScriptを置き換えるものです。
とはいえ、現時点のWebアプリの世界はJavaScriptで成り立っていますし、既に莫大な資産があります。
それらを放り出してしまうのはあまりに勿体無いので、TypeScriptでも活用していきたいところです。
そのためにTypeScriptは既存のJavaScript用資産を活用するための仕組みを持っています。
それが、型定義ファイルです。

通常のTypeScriptのコードの拡張子が .ts なのに対して、型定義ファイルは拡張子を .d.ts とします。
拡張子を .d.ts とすると、実装を含むようなコードを書くとtscがエラーにしてくれますので、ケアレスミス予防のためにも型定義ファイルは必ず .d.ts にします。

TypeScriptではJavaScriptの自由奔放(かつ、危険がてんこ盛り)の世界に後付で型を与えます。
元からTypeScriptで書かれている場合、実装と型定義を同時に書いているようなものなので、実装と型がズレて(つまりバグって)しまうことはありません。
一方、型定義ファイルは既に実装があるJavaScriptに後付で手書きで型をつけていくため、ズレる(バグる)可能性が大いに有ります。
そこのところを十分に気をつけないといけません。

== ツールを使って検索だ！

さて、まずは自分で型定義ファイルを作るよりも、既存のものを使ってみましょう。
有名どころはひと通り揃っています。

そのために、まずは tsd または dtsm というツールを導入しましょう@<fn>{dtsm}@<fn>{NuGet}。

//cmd{
# どちらか！
$ npm install -g tsd
$ npm install -g dtsm
//}

まずは、プロジェクトで使う型定義ファイルを記録するために依存関係保存用のファイルを生成します。

//cmd{
# どちらか！
$ tsd init
$ dtsm init
//}

ではpower-assertの型定義ファイルをダウンロードし、保存してみます。

//cmd{
# どちらか！
$ tsd query power-assert --action install --save --resolve

>> tsd 0.5.7

 - power-assert/power-assert.d.ts : <head> : 2014-11-14 02:03

   >> empower/empower.d.ts : <head> : 2014-11-14 02:03
   >> power-assert-formatter/power-assert-formatter.d.ts : <head> : 2014-11-14 02:03

>> running install..

>> written 3 files:

    - empower/empower.d.ts
    - power-assert-formatter/power-assert-formatter.d.ts
    - power-assert/power-assert.d.ts

$ dtsm install power-assert --save
power-assert/power-assert.d.ts
empower/empower.d.ts
power-assert-formatter/power-assert-formatter.d.ts
//}

デフォルトでは、typings/ フォルダに型定義ファイルが保存されます。
tsdとdtsmでは微妙に生成されるファイルが異なりますが、概ね以下のような構成になっているはずです。

//cmd{
$ tree typings
typings
├── empower
│   └── empower.d.ts
├── power-assert
│   └── power-assert.d.ts
└── power-assert-formatter
    └── power-assert-formatter.d.ts
//}

あとは、これら型定義ファイルを自分の使うコードから参照するだけでコンパイルが可能になります。
残念ながらライブラリの実体は含まれていないため、npmやbowerなどで別途取得する必要があるでしょう。

#@# TODO mochaも入れて、簡単なテストを書く例(型定義ファイルを使う例)を示す

//footnote[dtsm][dtsmは筆者(vvakame)が作っているツールで、まだあまり宣伝していないためユーザは少ないです。みんな使ってね！]
//footnote[NuGet][WindowsユーザにはNuGetというツールもあるのですが、全然知らないため割愛します。]

== 型定義ファイルを書こう

さて、型定義ファイルの取得の仕方はわかりました。
#@# TODO 使い方を書いたら↑の文言を変える
しかし、世の中にあるJavaScriptライブラリのうち、まだまだ型定義ファイルが書かれていないものは数多くあります。
特に、門外不出の社内内製ライブラリなどは誰も手をつけていない前人未到の地に違いありません。

しからば！自分で書くしかあるまいよ！
ぶっちゃけた話めんどくさいのですが、後々の安心・安全のための投資として割りきりましょう。

なお、自分で型定義ファイルを書く覚悟無しにTypeScriptをやるのは茨の道だと思いますので頑張ってください。
誰かがやってくれないと自分ではできません、なんて甘えた根性では型サバンナでは到底生きていけないのです(DefinitelyTypedメンテナの意見です。)

=== 型、実体、そして42。

TypeScriptはJavaScriptに対して後付けで型による制約を付け足した言語です。
そのため、JavaやC#のような最初から型ありきの言語より少し考え方が複雑です。
具体的に言えば、型と実体(値)というものが分かれています。

全てがTypeScriptで書かれているプログラムであれば、型と実体は基本的には一致しています。
クラスの定義を書いた時、JavaScriptプログラムとしてのクラス(OOPするためのコンストラクタ関数)と、TypeScriptで使う型としてのクラスが一度に誕生します。
これは非常に素直かつ簡単で、型と実体を1つの記述から作成しているので、この2つが乖離してしまうことはありません。

一方、JavaScriptで書いて、TypeScriptで型定義ファイルを当てて使う場合は実装と型が個別に定義されることになります。
そのため、型と実体の2つが分離してしまい、この2つの間に乖離が生じると(つまりバグると)コンパイルが通るのに実行時エラーが多発する、という有り様に成るわけです。
型定義ファイルを書いて"この変数は、あります！"と宣言したけれど、実際には存在せず実行時エラーになるというのはままある話です。

=== 良い型定義ファイル、悪い型定義ファイル

型定義ファイルにも、良し悪しがあります。
その基準は至って簡単です。

 1. 正しいライブラリの使い方を導くこと。
 2. 他のコードや型定義ファイルに意図せぬ干渉を引き起こさないこと。
 3. IDE上で使いやすいこと。

こんな感じです。

正しいライブラリの使い方を導く、というのは、裏を返せば間違った使い方ができないようにする、ということです。
これには、型と実体の定義に乖離が存在せず、コンパイルが通ったら実行時エラーが簡単には起こらないことも含まれます。

他のコードや型定義ファイルに意図せぬ干渉を引き起こさないこと、というのは、意図せぬインタフェースの統合などが起こったりしないことです。
このためには、あまりにも汎用的な名前を避けるだとか、可読性の高く理解しやすい型定義を作り干渉するかしないかを容易に判断できるようにすることも含まれます。

IDE上で使いやすいこと、というのは、Visual StudioやWebStorm上で入力補完の候補が不用意にたくさん出過ぎないようにして見通しのよい開発を助けることなどが含まれます。

これら3つを守ることが"良い品質であること"に繋がるというのは、TypeScript自体が型を使って間違ったコードを書きにくいようにするツールであると考えると納得できると思います。

人は誰しも"上手く書けないので仕方なく"悪い型定義を書いてしまうことがあります。
DefinitelyTypedにpull requestを送ってくれる人にもそういう人は多くいます。

これから説明するベストプラクティスを踏まえて、より良い型定義ファイルを作成できるように鍛錬していきましょう。

== 型定義ファイルのベストプラクティス

型定義ファイルを書く上でのベストプラクティスを解説していきます。
基本的には@<href>{http://definitelytyped.org/guides/best-practices.html,DefinitelyTypedのbest practives}@<fn>{dt-best-practice}と@<href>{http://www.typescriptlang.org/Handbook#writing-dts-files,公式のHandbookのWriting .d.ts files}@<fn>{official-handbook}に従っておけばよいです。
本書では、そこに書かれていることや筆者の経験則などを説明していきます。

//footnote[dt-best-practice][http://definitelytyped.org/guides/best-practices.html]
//footnote[official-handbook][http://www.typescriptlang.org/Handbook#writing-dts-files]

=== テキトーに、やろー！

一番最初にコレを書くのもどうかと思うのですが、まずは"使える"ようにすることが一番大切です。

型定義ファイルの品質の良さにこだわるあまり、完成しない・使いたいライブラリが使えない、というのが一番よくない状態です。
最終的に、良し悪しが分かるようになるにはTypeScript自体の理解度に大きく依存します。
TypeScriptを書き始めの頃は、品質は気にした所で後々粗が見えてくるのは避けられないのでまずは"使える"状態にすることを目指しましょう。

品質や"ライブラリ全体をカバーしている"かは気になるところではあります。
しかし、まずは使いたいところが使えればいいのです。
スゴいものになると、1万行超える型定義ファイルがあったり、3000行のはわりとごろごろしていたり…。そんなのを頑張って書いてると、余裕で日が暮れてしまいます@<fn>{atom-dts}。

まずは、使いたいところが、使える！
それで良いのです。
ドラゴン・ゲンドーソー＝先生のインストラクション・ワンを思い出しましょう。
「百発のスリケンで倒せぬ相手だからといって、一発の力に頼ってはならぬ。一千発のスリケンを投げるのだ！」
最初上手くできなくても、数をこなし学習を重ねれば、そのうち立派な型定義ファイルが書けるようになるでしょう。

//footnote[atom-dts][なお、筆者はGitHubの作っているエディタ、Atomの型定義ファイルを3日かけて書いたことがあります。アレがジゴクだ。]

=== インタフェースを活用する

インタフェースは大変使いやすいパーツです。
というのも、インタフェースは@<strong>{後から定義を拡張できる}という特性があるからです(@<list>{declaration-merging}、@<list>{declaration-merging-usage})。

//list[declaration-merging][定義を分割して書く]{
#@mapfile(../code/definition-file/declaration-merging.d.ts)
interface Foo {
    hello(): string;
}

// 同名のインタフェースを定義すると、合体される！
interface Foo {
    bye(): string;
}
#@end
//}

//list[declaration-merging-usage][定義が統合される！]{
#@mapfile(../code/definition-file/declaration-merging-usage.ts)
/// <reference path="./declaration-merging.d.ts" />

var foo: Foo;

foo.hello();
foo.bye();
#@end
//}

この通り、別々に定義したインタフェースが1つに統合されています。
これを利用することで、既存の型であろうとも拡張が可能な場合があります。

例を1つ見てみましょう。
Array#find は、指定した方法に基づき要素を1つ探す関数です。
TypeScript 1.3.0ではデフォルトの型定義ファイル(lib.d.ts)にはまだこのメソッドが定義されていません。
そのため、Arrayインタフェースを拡張する形でコンパイルを通せるようにしてみましょう(@<list>{array-find})。

//list[array-find][Array#find を生やす]{
#@mapfile(../code/definition-file/array-find.ts)
// TypeScript 1.3.0 では Array.prototype.find の定義はまだ存在していない
interface Array<T> {
    find(callback: (e: T, idx: number, array: T[]) => boolean, thisArg?: any): T;
}

var array = [1, 2, 3];

// 最初に見つかる奇数を得る
array.find(v => v % 2 === 1);
#@end
//}

あとは、実装側を@<href>{https://github.com/paulmillr/es6-shim/,es6-shim}@<fn>{es6-shim}などで埋めてやれば古いブラウザでも利用可能になるでしょう。

この手法は、他人が作った型定義ファイルを拡張する場合にも活用することができます。
相乗りできるのであれば遠慮なく乗っかっていってしましょう。

//footnote[es6-shim][https://github.com/paulmillr/es6-shim/]

=== 幽霊モジュール

幽霊モジュール@<fn>{ghost-module}という考え方があります。

内部モジュールを作ったとしても、即座に実体が生成されるとは限りません。
内部モジュールが抱えるのがインタフェースのみである場合、実体がある扱いにはならないのです(@<list>{ghost-module-invalid})。

//list[ghost-module-invalid][幽霊モジュール]{
#@mapfile(../code/definition-file/ghost-module-invalid.ts)
declare module ghost {
    interface Test {
        str: string;
    }
}

// 型としては普通にアクセスできる
var test: ghost.Test;
test.str;

// 実体としては存在していない！
// ghost-module-invalid.ts(13,17): error TS2304: Cannot find name 'ghost'.
var notExists = ghost;
#@end
//}

これを活用して、大量のインタフェースを持つようなライブラリの定義をひとまとまりにできます。

実際の例を見てみましょう。
@<list>{jquery-without-ghost-module}はjQueryの型定義ファイルの抜粋(&一部改変)です。

//list[jquery-without-ghost-module][実際のjQueryの型定義の例]{
#@mapfile(../code/definition-file/jquery-without-ghost-module.d.ts)
interface JQuery {
    addClass(className: string): JQuery;
    html(htmlString: string): JQuery;
    val(): any;
    empty(): JQuery;
    append(content1: JQuery, ...content2: any[]): JQuery;
    appendTo(target: JQuery): JQuery;
}

interface JQueryStatic {
    ajax(settings: JQueryAjaxSettings): any;
    (selector: string, context?: Element): JQuery;
    (element: Element): JQuery;
}

interface JQueryAjaxSettings {
    data?: any;
    type?: string;
    url?: string;
}

interface JQueryPromise<T> {
    state(): string;
    then<U>(fullfill: (value: T) => U, reject?: (...reasons: any[]) => U): JQueryPromise<U>;
}

interface JQueryDeferred<T> extends JQueryPromise<T> {
    reject(...args: any[]): JQueryDeferred<T>;
    resolve(value?: T, ...args: any[]): JQueryDeferred<T>;
}

declare var $: JQueryStatic;
#@end
//}

トップレベルに複数の型がいくつも散乱してしまうのがよくありません。
それに、@<code>{JQuery}というprefixが乱舞していて目を惑わせます。
ライブラリ内部のAPI同士の参照でも引数や返り値にプリフィクスが必要なのはめんどうくさいです。
IDE上で型注釈を手書きする時も候補がたくさんサジェストされてしまうことでしょう。

これを幽霊モジュールを使って書きなおしてみます。

//list[jquery-with-ghost-module][幽霊モジュールを使ってみた]{
#@mapfile(../code/definition-file/jquery-with-ghost-module.d.ts)
declare module jquery {
    interface Element {
        addClass(className: string): Element;
        html(htmlString: string): Element;
        val(): any;
        empty(): Element;
        append(content1: Element, ...content2: any[]): Element;
        appendTo(target: Element): Element;
    }

    interface Static {
        ajax(settings: AjaxSettings): any;
        (selector: string, context?: Element): Element;
        (element: Element): Element;
    }

    interface AjaxSettings {
        data?: any;
        type?: string;
        url?: string;
    }

    interface Promise<T> {
        state(): string;
        then<U>(fullfill: (value: T) => U, reject?: (...reasons: any[]) => U): Promise<U>;
    }

    interface Deferred<T> extends Promise<T> {
        reject(...args: any[]): Deferred<T>;
        resolve(value?: T, ...args: any[]): Deferred<T>;
    }
}

declare var $: jquery.Static;
#@end
//}

インタフェース名が短く、かつわかりやすくなりました。
そうそう、こういうのでいいんだよこういうので！

もちろん、無理に幽霊モジュールを使う必要はありません。
クラスや変数、関数などを持ち、通常の実体のあるモジュールが存在している場合は、そのモジュールに相乗りしていってしまったほうが楽でしょう。

…なんでDefinitelyTyped上にある型定義ファイルでそうなってないものが多いかって？
良い質問です。
一つ目は幽霊モジュールの認知度が低いこと、もう一つは型定義ファイルの大幅な書き換えは互換性の破壊を生み出すからです。
先で説明しましたが、インタフェースは定義の統合ができます。
この性質を利用して定義の拡張を行っているので、うかつにJQueryStaticからjquery.Staticに変更するとjQueryの型定義に依存している様々なライブラリの色々なところが壊れてしまうのです。
特にjQueryプラグインとかはその方法で型定義するのでその量たるや…。

ともあれ、過去の定義との互換性を壊す事に繋がるため、途中から幽霊モジュールに切り替えるのは難しい場合があります。
可能であれば最初から幽霊モジュールを使うようにしましょう。
将来的には、このパターンの検出にはtslintなどで機械的に行えるようにしたいところですね。

//footnote[ghost-module][TypeScriptリファレンスでは非インスタンス化モジュールという名前で紹介しました。その後、DefnitelyTypedのbest practiceでghost moduleと表記されたのでそちらに統一。]

=== なんでもかんでもインタフェースにしてはならない

おうお前少し前の文章であんだけインタフェースを持ち上げといてこれかぁ！？
と、思われたかもしれませんが、なんでもかんでも乱用すればいいってものではありません。

具体的に、モジュール様の構造をインタフェースを使って作ってはいけません(@<list>{module-by-interface-bad})。

//list[module-by-interface-bad][インタフェースでモジュールを表現してしまう。何故なのか…]{
#@mapfile(../code/definition-file/module-by-interface-bad.d.ts)
interface Foo {
    bar: FooBar;
}

interface FooBar {
    buzz: FooBarBuzz;
}

interface FooBarBuzz {
    str: string;
}

declare var foo: Foo;

// foo.bar.buzz.str という使い方ができる。わかりにくくてユーザは死ぬ。
#@end
//}

この型定義ファイルを読み解いて一瞬で使える人は、元のJavaScriptコードを熟知している人だけでしょう。
少なくとも、この型定義ファイルをヒントに実際のコードを書くことは大いなる苦痛が伴います。
俺は絶対使わんぞ！
普通に、@<list>{module-by-interface-good}のように書くのだ！

//list[module-by-interface-good][素直にこうしよう]{
#@mapfile(../code/definition-file/module-by-interface-good.d.ts)
// 普通にコレでいいだろ！！
declare module foo.bar.buzz {
    var str: string;
}
#@end
//}

普通、こんな型定義ファイルを書こうとは思わないと思いますが、こういうコードを書きたくなってしまう時が稀にあります。
具体的には、@<list>{callable-module-usage}のように関数としても呼べるし、内部モジュールのようにも振る舞うオブジェクトの型定義を作成したい時です。

//list[callable-module-usage][関数・内部モジュール どっちなの？]{
#@mapfile(../code/definition-file/callable-module-usage-invalid.ts)
// assertは関数としても呼べるしモジュールのようにも見える
assert(foo === "foo");
assert.ok(value);
#@end
//}

呼び出し可能で、プロパティを持つ。ふむ、じゃあ@<list>{callable-module-bad1}か、@<list>{callable-module-bad2}だ！

//list[callable-module-bad1][こうしてしまいたい、気持ち]{
#@mapfile(../code/definition-file/callable-module-bad1.d.ts)
declare var assert: {
    (value: any): void;
    ok(value: any): void;
}
#@end
//}

//list[callable-module-bad2][匿名型注釈よりはマシ]{
#@mapfile(../code/definition-file/callable-module-bad2.d.ts)
declare var assert: Assert;

interface Assert {
    (value: any): void;
    ok(value: any): void;
}
#@end
//}

たしかに、この定義でも動きます(正直、assert関数だけの定義だとこのままでもいい気がしますが…)。

しかし、これには別の良いやり方があるのです(@<list>{callable-module-good})。

//list[callable-module-good][関数と内部モジュール 両方やらなきゃいけないのが(ry]{
#@mapfile(../code/definition-file/callable-module-good.d.ts)
declare function assert(value: any): void;
declare module assert {
    function ok(value: any): void;
}
#@end
//}

関数と内部モジュールを同名で宣言できるのです。
これの効能は、層構造を素直に表現できることと、前項で説明した幽霊モジュールの書き方を併用できるところです。

この手法は、実際に@<href>{https://github.com/borisyankov/DefinitelyTyped/blob/master/power-assert/power-assert.d.ts,power-assertの型定義ファイル}@<fn>{power-assert-dts}でも利用されています。
@<list>{power-assert-abst}に抜粋&改変したものを示します。

//list[power-assert-abst][関数と内部モジュール 両方やらなきゃいけないのが(ry]{
#@mapfile(../code/definition-file/power-assert-abst.d.ts)
declare function assert(value: any, message?: string): void;
declare module assert {

    export function deepEqual(actual: any, expected: any): void;
    export function notDeepEqual(acutal: any, expected: any): void;

    export interface Options {
        assertion?: any;
        output?: any;
    }

    export function customize(options: Options): typeof assert;
}
#@end
//}

外部に公開されている関数は@<code>{assert}のみで、そこに追加でプロパティが生えている形式です。
実体のある要素(関数)があるため、幽霊モジュールにはなりませんが、Optionsインタフェースが上手く取り込まれています。
余計な名前を階層の浅いところにバラ撒かず、厳密さも損なっていません。
この書き方は、案外よく登場するパターンなので覚えておくと良いでしょう。

実は、このやり方は型定義ファイルだけではなく、通常のTypeScriptコードでも使えます(@<list>{callable-module-ts})。

//list[callable-module-ts][関数が先、内部モジュールは後！絶対！]{
#@mapfile(../code/definition-file/callable-module.ts)
function test() {
    return "test!";
}
module test {
    export function func() {
        return "function!";
    }
}
#@end
//}

コンパイル結果の@<list>{callable-module-js}を見ると、なぜ関数が先、内部モジュールが後、という決まりになっているかがわかりますね。

//list[callable-module-js][コンパイル結果を見れば、理由がわかる。これ、正しいJSだ！]{
#@mapfile(../code/definition-file/callable-module.js)
function test() {
    return "test!";
}
var test;
(function (test) {
    function func() {
        return "function!";
    }
    test.func = func;
})(test || (test = {}));
#@end
//}

//footnote[power-assert-dts][https://github.com/borisyankov/DefinitelyTyped/blob/master/power-assert/power-assert.d.ts]

=== クラスを定義するには？

普通に定義すればええやろ！！と思うかもしれませんが、現在のTypeScriptはなかなか難しい問題を抱えています。
先に、どういう選択肢が存在するかを見てみましょう(@<list>{declare-class})。

//list[declare-class][素直にクラス定義 vs インタフェース+変数]{
#@mapfile(../code/definition-file/declare-class.d.ts)
// A. 普通にクラスを定義する
declare class TestA { }

// B. クラスの分解定義 変数 + インタフェース2つ
declare var TestB: TestBStatic;
interface TestBStatic {
    new (): TestB;
}
interface TestB {
}
#@end
//}

こんな感じです。
ぱっと見、普通にクラス定義をするほうが素直ですね。

しかし、それぞれの手法にメリット・デメリットがあるのです。
そのライブラリのユースケースにとって、どちらが適切か見極めねばなりません。

==== 素直にクラスのメリット・デメリット

 * ライブラリ利用時に普通に継承できる
 * 定義の拡張ができない
 ** 別ライブラリが(プラグインなどで)拡張する設計のライブラリには向かない
 * 別途インタフェースの実装を型定義に盛り込む時めんどくさい(定義の二重記述が必要)

@<list>{declare-vanilla-class-invalid}みたいな感じ。

//list[declare-vanilla-class-invalid][クラスで定義]{
#@mapfile(../code/definition-file/declare-vanilla-class-invalid.ts)
declare class BaseA {
    str: string;
}
// ○利用時に普通に継承できる！
class Inherit extends BaseA {
    number: number;
}
// × クラスはopen endedじゃないので定義の拡張ができない…
// error TS2300: Duplicate identifier 'BaseA'.
declare class BaseA {
    num: number;
}

interface FooListener {
    on(eventName: string, callback: (data: any) => void): void;
}
declare class FooListenerImpl implements FooListener {
    // インタフェースの要素全部書かないとコンパイル通らない…
    on(eventName: string, callback: (data: any) => void): void;
}
#@end
//}

==== インタフェース+変数に分解

 * ライブラリ利用時に継承できない
 ** new するだけの利用法なら特に不便ではない
 * インタフェース定義の統合が使えるので別ライブラリの拡張にも対応できる！
 * インタフェースを実装するのが(継承するだけなので)めっちゃ簡単

@<list>{declare-decompose-class-invalid}みたいな感じ。

//list[declare-decompose-class-invalid][インタフェース+変数で定義]{
#@mapfile(../code/definition-file/declare-decompose-class-invalid.ts)
var BaseA: BaseAStatic;
interface BaseAStatic {
    new (): BaseA;
}
interface BaseA {
    str: string;
}
// × 利用時に継承できない…(しょせんインタフェース
// error TS2311: A class may only extend another class.
class Inherit extends BaseA {
    number: number;
}
// ○ インタフェースは定義の拡張ができる！！
interface BaseA {
    num: number;
}

interface FooListener {
    on(eventName: string, callback: (data: any) => void): void;
}
var FooListenerImpl: FooListenerImplStatic;
interface FooListenerImplStatic {
    new (): FooListenerImpl;
}
interface FooListenerImpl extends FooListener {
    // FooListenerの要素何も転記しなくて済む！
}
#@end
//}

==== まとめ

クラスの型定義がopen endedになって定義を拡張可能にして、@<href>{https://github.com/Microsoft/TypeScript/issues/371,インタフェースのオプショナルな実装}@<fn>{optional-interface}サポートしてくれたらクラス定義で全部賄える！

今のところはどうしようもないので、用途に応じて適切なほうを選びましょう。

//footnote[optional-interface][https://github.com/Microsoft/TypeScript/issues/371]

=== オーバーロードを上手く使おう！

正しいライブラリの使い方を導くこと。
を心に秘めて、@<list>{use-overload}を見てください。
どれが一番わかりやすいですか？

//list[use-overload][普通に使えます]{
#@mapfile(../code/definition-file/use-overload.ts)
// 同じ実装に対して、どの型定義が一番便利かな？

// getの時 setの時 仕様が違うことがよくわかる
declare function valueA(value: any): void;
declare function valueA(): any;

// setの時も値が取れるっぽい？
declare function valueB(value?: any): any;

// 詳細が不明だ…！
declare var valueC: Function;
#@end
//}

答え：一番最初のやつ。

JavaScriptのライブラリは1つの関数に様々な使い方をさせようとする場合が多くあります。
つまり、1つの関数が複数の顔を持つということです。
その顔1つ1つに個別の型定義を割り振ってやるテクニックをオーバーロードと呼びます。

なお、普通にTypeScriptコードを書く時にオーバーロードはあまり使わないのが良いスタイルです。
実装が煩雑になっちゃうからね！素直にメソッドを分けましょう。

union typesが使えるようになると、@<list>{overload-vs-union-types}のように書くこともできます。
簡単な例だとunion typesのほうがよいと思いますが、このケースではどっちがいいかは、今の知見ではまだわからないですね。

//list[overload-vs-union-types][うーん、どっちがいいかは難しい]{
#@mapfile(../code/definition-file/overload-vs-union-types.ts)
// union types 以前
declare function hello(word: string): string;
declare function hello(callback: () => string): string;

hello("TypeScript");
hello(() => "function");

// union types 以後 好みの問題かもね
declare function bye(word: string | { (): string; }): string;

bye("JavaScript");
bye(() => "function");
#@end
//}

=== 外部モジュールの定義の統合

@<strong>{利用可能になったバージョン 1.3.0}

あんまり言及される事がないのでここで触れておきます。
1.1.0-1までの時代は、外部モジュールがopen endedじゃありませんでした。
1.3.0からはopen endedになったので、@<list>{external-module-declaration-merging}と@<list>{external-module-declaration-merging-usage}のようなコードが書けます。
めでたい。

//list[external-module-declaration-merging][これ、1.1.0時代はできなかったのよね]{
#@mapfile(../code/definition-file/external-module-declaration-merging.d.ts)
// 外部モジュールの定義の統合ができます！！
declare module "foo" {
    var str: string;
}

declare module "foo" {
    var num: number;
}
#@end
//}

//list[external-module-declaration-merging-usage][普通に使えます]{
#@mapfile(../code/definition-file/external-module-declaration-merging-usage.ts)
/// <reference path="./external-module-declaration-merging.d.ts" />

import foo = require("foo");
foo.str;
foo.num;
#@end
//}

ちなみに、それまでは内部モジュールとして拡張ポイントを@<href>{https://github.com/borisyankov/DefinitelyTyped/blob/e3b19b66f2750b10f262a698098cabbf210f7f2a/express/express.d.ts#L15,外出しして用意}@<fn>{extract-module}し、@<href>{https://github.com/borisyankov/DefinitelyTyped/blob/e3b19b66f2750b10f262a698098cabbf210f7f2a/passport/passport.d.ts#L8,他所で拡張する}@<fn>{use-internal-module}という頑張り方をしていました。
泣けますね。
既存ライブラリにメソッドを生やすという黒魔術はNode.js上でもできてしまうのが辛いところです。
とはいえ、1.3.0以降は型定義の作成が多少楽になったのが嬉しいところです。

//footnote[extract-module][https://github.com/borisyankov/DefinitelyTyped/blob/e3b19b66f2750b10f262a698098cabbf210f7f2a/express/express.d.ts#L15]
//footnote[use-internal-module][https://github.com/borisyankov/DefinitelyTyped/blob/e3b19b66f2750b10f262a698098cabbf210f7f2a/passport/passport.d.ts#L8]

=== any と {} と Object

もし、型定義ファイルを書いていて具体的な型がわからない場合、調べるのがめんどくさい場合、なんでもいいばあい、そういう時にはanyを使いましょう。

たまに、Objectを指定する人がいます。
これはJavaScriptの仕様として、プロトタイプチェーンの頂点にいるのでObjectを使おう！と思うのでしょうが、これはやめたほうがよいでしょう。

関数の引数に Objectや {} を指定するのは、結局どのような引数でも受け入れてしまいます。
本当にどのような値でも受け入れる関数であれば、anyにするべきです。

関数の返り値にObjectや {} を指定するのは、結局どのようなプロパティも存在しないため型アサーションでもって適切な型にするしかありません。
これは、anyを指定するのと危険度としては同程度に危険です。
素直にanyを使いましょう。

筆者は今のところ、Objectや {} が型注釈として適切な場面を見たことがありません。
もし、そういう存在を知っている！という方が居られましたら、ぜひご教示ください。
大抵の場合は、適切な型を定義してそちらを参照するほうが優れているはずです。

そして、anyが気になるのであれば、よくよく調べて適切な型定義を与えるようにしましょう。

=== ドキュメントから書き起こす

もし、ライブラリにしっかりしたドキュメントがあるのであれば、実装コードから型定義ファイルを起こすのではなく、ドキュメントをベースに作成しましょう。
Visual StudioなどのIDEでは、型定義ファイル上に書かれたJSDocコメントも利用時に表示してくれる場合があります。
そのため、型定義を起こしつつ、あわせてJSDocを記述していくとよいでしょう。

少なくとも、型定義には既に書き起こしてあるのにドキュメント中に書かれている利用例がコンパイルに通らないような型定義は悪い型定義であるといえます。
サンプルをテスト用コードとしてTypeScriptコードに移植し、ドキュメント通りの記述が可能かも確かめるとよいでしょう。

とはいえ、世の中ドキュメントにコストをあまり書けることのできないプロジェクトも多くあります。
その場合、コードから型定義ファイルを起こすことになるのは仕方のないことです。

なお、DefinitelyTypedのjqueryの型定義ファイルを熱心に面倒みてくれているJohn Reillyは特にドキュメントとの整合性を熱心に見るので、もしjQueryのドキュメント自体が間違っている場合はjQueryのドキュメントを治すところから始めるとよいでしょう。
コントリビュートの輪！

=== コールバック関数の引数を無闇に省略可能(optional)にしない

まずは例を見てみましょう(@<list>{callback/basic})。

//list[callback/basic][optionalはもしかしたら値がない事を表す]{
#@mapfile(../code/definition-file/callback/basic.ts)
// 良い例
declare function onClick(listener:(e:MouseEvent) => void):void;
// 悪い例
declare function onClickOpt(listener:(e?:MouseEvent) => void):void;

// 使ってみよう！
onClick(e => {
    // ここでのeは必ず実体がある
    console.log("clicked!" + e.srcElement.textContent);
});
onClickOpt(e => {
    // ここでのeはundefinedかもしれない… チェックしなくていいの？
    console.log("clicked!" + e.srcElement.textContent);
});

// 引数を無視するのは自由 optionalにする理由にはならぬ！
onClick(()=> {
    console.log("clicked!");
});
onClickOpt(()=> {
    console.log("clicked!");
});
#@end
//}

両方とも、クリックイベントをハンドリングするための関数を型定義として書き起こしたものです。
onClickはeが省略不可、onClickOptはeが省略可能(optional)になっています。
これがは、onClickOptではeがundefinedになるかもしれない事を表します。
eがundefinedかもしれないなら、if文とかで中身があるかチェックしなくていいの？という不安が生じます。

執筆時点ではこれを検出してくれる程度に頭のいいlintツールは筆者の知るかぎり存在していません。
ですが、正しい意思表示を行うためにも引数が確実に与えられると分かっている場合は、省略可能にしないようにしましょう。

optionalとは、値が渡されるかどうかの指標であって、コールバックを受け取った側が使うかどうかではないのです。
そのことに留意しておきましょう。

=== インタフェースのプリフィクスとして I をつけるのはやめろ！

って@<href>{http://www.typescriptlang.org/Handbook#writing-dts-files,明記}@<fn>{writing-dts-files}された。

C#やJavaよりも、広い範囲でインタフェースが利用されるので"実装を強制させるパーツ"扱いしてはいけないからである。

らしいが、そもそもC#の文化持ち込んで I つけるのやってたのMicrosoftさんじゃないんですかぁー！？
という気持ちになるがまぁ広い心で受け入れていこうなのだ。
過去にもboolをbooleanに変えたり、先頭大文字のメソッド名を先頭小文字にしたり、C#文化をWeb界隈にあわせようとしてくれてますからね…。

そんなこんなで、まだ微妙に古いスタイルが残ってたりするのでTypeScriptコンパイラのコードを見てTypeScriptのコードスタイルを勉強してはいけないゾ☆
本当に色々なスタイルが混在していてヤバイんですよ。1.4.0で内部APIが公開されるまでには修正されると思うのですけれど。

//footnote[writing-dts-files][http://www.typescriptlang.org/Handbook#writing-dts-files]

=== ちょっと小難しい export 句の使い方

インタフェースやクラスのインスタンス単体を外部モジュールの外側に見せたい場合、@<list>{export-sample1}のように書きます。

//list[export-sample1][実はインタフェースFooも外から見えない]{
#@mapfile(../code/definition-file/export-sample1.d.ts)
declare module "foo" {
    interface Foo {
        num: number;
    }

    // この _ は外部からは参照できない。exportしてないので。
    var _:Foo;
    export = _;
}
#@end
//}

呼び出し側では@<list>{export-sample1-usage}のように使います。
importした値がインタフェースFooのインスタンスになっていることがわかります。

//list[export-sample1-usage][使う時。インタフェースFooのインスタンスが得られる。]{
#@mapfile(../code/definition-file/export-sample1-usage.ts)
/// <reference path="./export-sample1.d.ts" />

// f は "foo" の Fooのインスタンス だよ！
import f = require("foo");
f.num;
#@end
//}

よくやりがちな誤りは@<list>{export-sample2}みたいな書き方をしてしまうことです。
インタフェースのインスタンスをexportしたつもりが型がexportされてしまうのです。
そして@<list>{export-sample2-usage-invalid}のようなエラーに出くわすことになります。

//list[export-sample2][それは値ではなくて型だけ輸出しているぞ！]{
#@mapfile(../code/definition-file/export-sample2.d.ts)
declare module "foo" {
    interface Foo {
        num: number;
    }

    // よくやりがちな過ち
    export = Foo;
}
#@end
//}

//list[export-sample2-usage-invalid][ｱｰｯ! らめぇ！]{
#@mapfile(../code/definition-file/export-sample2-usage-invalid.ts)
/// <reference path="./export-sample2.d.ts" />

// f は "foo" の Fooそのもの だよ！
import f = require("foo");

// export-sample2-usage-invalid.ts(7,1): error TS2304: Cannot find name 'f'.
f.num;

// この書き方は正しい
import Foo = require("foo");
var foo: Foo;
foo.num;
#@end
//}

こういう悲しい目を回避するには、型定義ファイルのテストが有効です。
とりあえず型定義ファイル書いたら適当なユースケースに当てはめて意図通りコンパイルできるか確かめてみましょう。

TypeScriptでコードを書く上は、1ファイル1インタフェースorクラスの図式でプロジェクトを構成することはありえます。
一方、インタフェース単体をexportするのって、既存JavaScriptファイルに対する型定義ファイルではありえない操作と言えます。
こういう操作をtslintで検出できるようにすると便利そうですね。
将来への課題としておきましょう。

=== 最終チェック！

やった！型定義ファイルが書けたぞ！
己の出来高に満足する前に、もう少しだけやっておきたいことがあります。
それが、--noImplicitAnyをつけての試しコンパイルとtslintによるチェックです。

==== --noImplitictAny

TypeScriptコンパイラの最重要オプション、--noImplicitAnyを使って型定義ファイルをコンパイルしてみましょう。
@<list>{noImplicitAny/basic-invalid}のような、メソッドの返り値の型を書き忘れた！という脇の甘いコードを突っ込んでみます。

//list[noImplicitAny/basic-invalid][メソッドの返り値を書き忘れた！]{
#@mapfile(../code/definition-file/noImplicitAny/basic-invalid.d.ts)
declare class Sample {
    // 返り値の型を指定し忘れている！
    // error TS7010: 'method', which lacks return-type annotation,
    //               implicitly has an 'any' return type.
    method();
}
#@end
//}

//cmd{
$ tsc --noImplicitAny definition.d.ts
definition.d.ts(3,5): error TS7010: 'method', which lacks return-type
    annotation, implicitly has an 'any' return type.
//}

返り値の型が書いていないため暗黙的にanyになってしまいました。
これに対して、それはあかん！とコンパイラが教えてくれます。
anyが紛れ込んできて、型チェックが意味を成さなくなるのはTypeScriptコードを意義が薄れてしまいます。
型定義ファイルを書くときも、通常の開発時も、常に--noImplicitAnyを使うようにしましょう。

==== tslint

lintというプログラムの種類があります。
ざっくり、プログラムを静的に解析してバグになりそうな箇所や悪いコードスタイルを見つけてくるツールのことを指します。

TypeScriptではtslintというプログラムが一般的に使われています。
tslintのリポジトリは@<href>{https://github.com/palantir/tslint,こちら}@<fn>{tslint-repo}です。

tslintはコンパイルだけでは見つけきれない、悪い匂いのするコードを検出してくれます。
例を見てみましょう(@<list>{tslint/basic})。

//list[tslint/basic][ん？何かおかしなコードがあるぞ？]{
#@mapfile(../code/definition-file/tslint/basic.ts)
// name must be in pascal case
class foo {
}
// unused variable: 'bar'
var bar: any;
#@end
//}

このコードの悪いところは、クラス名が先頭大文字じゃない！一回も参照していない変数がある！というところです。
その両方とも、tslintは検出してくれています。

tslintは必ず設定ファイルを必要とします。
今のところ、TypeScript界における統一見解は存在していないので、@<href>{https://github.com/palantir/tslint/blob/master/tslint.json,tslintが使ってる設定ファイル}@<fn>{tslint-example-config}を参考にしてみるのがよいかもしれません。

#@# TODO 全設定項目解説とおすすめ設定を書きたいなぁ

//footnote[tslint-repo][https://github.com/palantir/tslint]
//footnote[tslint-example-config][https://github.com/palantir/tslint/blob/master/tslint.json]

== Let's contribution!

ようこそ！@<href>{https://github.com/borisyankov/DefinitelyTyped,DefinitelyTyped}@<fn>{dt}へ！
メンテナのvvakameです。

DefinitelyTypedでは様々な型定義ファイルを取り揃えてございます！
世界中の人々が作った型定義ファイルは集積され、tsdや、NuGetや、dtsmなどを介して広く利用されています。

貴方が作った型定義ファイルも、DefinitelyTypedでホストして世界中の人々に使ってほしいとは思いませんか？
もしくは、あなたがいつも使っている型定義ファイルのバグを治したい…そんな気持ち。

全て、DefinitelyTypedにぶちまけましょう！

本書を読んでいただいた紳士淑女の皆様はたぶん、良い感じの品質の型定義ファイルが書けるようになったことと思うんですよね。
品質と時間のトレードオフを考えて、いい感じの型定義ファイルを提供していただきたく存じます。

DefinitelyTypedはGitHub上のリポジトリなので、追加・修正についてはpull requestをご利用ください。

具体的にpull requestを送るまでの流れは以前@<href>{http://qiita.com/vvakame/items/1980d4b6cc222e03fdcb,Qiitaに書いた}@<fn>{guide-on-qiita}ので、そちらを参照していただけると幸いです。

#@# TODO 転記して再構成したい…

ここでは、筆者がDefinitelyTypedに送られてきた時にどういう考えでpull requestのレビューをしているかを示したいと思います。
あくまで、ここに書いてあることは筆者固有の観点なので、レビュワーによって別の事を言われる場合もあるでしょう。
実際にpull requestを送ってみて、ここに書いてある以外の理由で拒否されたとしても、そこは実際のレビュワーの言うことを尊重して議論していただきたいと思います。

とはいえ、メンテナは全員@<href>{http://definitelytyped.org/guides/contributing.html,DefinitelyTypedのContribution guide}@<fn>{dt-contrib-guide}は読んでいるはずなので、こちらには目を通しておくとよいでしょう。

//footnote[dt][https://github.com/borisyankov/DefinitelyTyped]
//footnote[guide-on-qiita][http://qiita.com/vvakame/items/1980d4b6cc222e03fdcb]
//footnote[dt-contrib-guide][http://definitelytyped.org/guides/contributing.html]

=== 新規型定義ファイルの追加のレビューの観点

まずは今までなかった、新しいライブラリに対する型定義ファイルのレビューの観点を解説していきます。

 1. CIが通っているか
 2. npmまたはbowerに公開されている名前と同じになっているか 公開されていない場合は競合が発生しないか
 3. テストが存在しているか
 4. 幽霊モジュールを使ったほうが構造が綺麗になるか

だいたいこんな感じです。

CIが通っているか。
これは、ヘッダが定められた形式で書かれているか、--noImplicitAny付きで型定義ファイルやテストがコンパイルできるか、を主に見ています。

npm, または bower に公開されている名前通りか。
これは、そのまんまでnpmに公開されているライブラリはnpmで公開されている名前と同一のディレクトリ名・ファイル名にします。
もし、npmに公開されていない場合は適当に名前を選ぶしかありませんが、同名の別のライブラリがnpm上に存在していないかなどをチェックしています。

逆に、ここに書かれていないことはあまり見ていません。
例えば、ライブラリの実装全体に対する、型定義ファイルのカバー率や、anyの多さなどはあまり見ていません。
それらは後から別の、それを必要とした人が補ってくれる可能性があるからです。
別に一人でやりきらなくてもいいよな！という発想ですね。
もちろん最初に高品質高カバー率のものが出てきたほうが やりおる！ とは思います。

なお、レビュワー次第ですがJSDocがきっちり書かれているか、というのを見る人もいます。
筆者はWebStormで開発していてJSDocの恩恵が薄いため、自然とJSDocをあまり重要視しなくなってしまいました。
とはいえ、きちんとドキュメントから転記などしてあるものが送られてきた時はやはり感心します。

=== 既存型定義ファイルの修正のレビューの観点

 1. CIが通っているか
 2. 破壊的変更が含まれていないか
 3. ライブラリ本体のドキュメントまたは実装からその変更内容が妥当であるかの検証

これだけです。
新規追加の場合は比較的レビューがガバガバなのですが、既存のものの変更は既に利用している人に影響があるため、勢い慎重になってしまいます。
そのため、結構レビューに時間が取られて辛いので、ドキュメントや実装が確認できるURLを添えてくれると大変嬉しいです。

いくつか補足しましょう。

破壊的変更が含まれていないか。
例えば、コードスタイルの変更(インタフェースのプリフィクスにIをつける、つけない など)や、幽霊モジュールを使っていないスタイルから使っているスタイルへの変更など。
または、別に間違っていないメソッドなどの型定義から別表現への書き換えなど。
これらはレビュアーが妥当かどうかを判断します。
大抵、判断できないのでヘッダに書いてある Definitions by: に名前が書いてある人達にGitHub上でmentionを飛ばして相談します。
型定義ファイルを作った人は、大抵の場合実際に使っている人、つまり破壊的変更を加えられると困る人達です。
ですので、もしある型定義ファイルに変な変更が入りそうな場合、きっと事前に相談してほしいに違いないと考えるわけです。

次に、変更内容が妥当であるかの検証 です。
大抵は筆者が該当ライブラリのドキュメントまたは実装を読んで正しいかどうかをverifyします。
しかし、大規模な更新やライブラリ固有の事情が入り込む場合、レビューできない or めっちゃくちゃ大変であると判断した場合は前項同様、 Definitions by: の人たちにレビューの依頼を投げます。

では、皆様のpull request、お待ちしています！
