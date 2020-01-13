/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

/*
 * std::array や std::pair や std::tuple を使うと、
 * 一部のメンバ関数がインライン展開されずに普通の関数として実体化される。
 *
 * この時、 AVX1 でコンパイルしたメンバ関数の実体が共有されると、
 * SSSE3 でコンパイルした関数から呼ばれて illegal instruction となる。
 *
 * 以下のようなメンバ関数を一切持たない POD (plain old data) 型だと、
 * 上記のような問題は発生しない。（コンパイラに依存した挙動かもしれないが）
 */

template<typename T>
struct VECTOR_RGB
{
	T g, b, r;
};

template<typename T>
struct VECTOR_RGBA
{
	T g, b, r, a;
};

template<typename T>
struct VECTOR_YUV422
{
	T y0, y1, u, v;
};


template<typename T>
struct VECTOR2
{
	T v0, v1;
};

template<typename T>
struct VECTOR3
{
	T v0, v1, v2;
};

template<typename T>
struct VECTOR4
{
	T v0, v1, v2, v3;
};
