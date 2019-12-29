#pragma once
#include <vector>
#include <memory>

template<typename T> class resizable_array
{
public:
	class iterator {
	public:
		iterator(const iterator& it) : ar_(it.ar_), position_(it.position_) {}
		iterator(const resizable_array& ar, size_t position) : ar_(ar), position_(position) {}
		T& operator*() { return ar_.at(position_); }
		bool operator!=(const iterator& it) { return position_ != it.position_; }
		iterator operator++() { position_++; return *this; }
	private:
		const resizable_array<T>& ar_;
		size_t position_{ 0 };
	};
	resizable_array(size_t chunk_size = 100) : chunk(chunk_size) {}
	void push_back(const T& x);
	T& operator[](size_t index) const { return references[index / chunk][index % chunk]; }
	T& at(size_t index) const { if (index < size_) { return (*this)[index]; } else throw; }
	iterator begin() const { return iterator(*this, 0); }
	iterator end() const { return iterator(*this, size_); }
	size_t size() const {return size_;}
private:
	std::vector<std::unique_ptr<T[]>> references;
	void resize(); // Creates new space for item
	size_t chunk;
	size_t size_{ 0 };
};

template<typename T> void resizable_array<T>::push_back(const T& x)
{
	resize();
	references[references.size() - 1][size_++ % chunk] = x;
}

template<typename T> void resizable_array<T>::resize()
{
	if (size_ + 1 > references.size()* chunk)
		references.push_back(std::make_unique<T[]>(chunk));
}

