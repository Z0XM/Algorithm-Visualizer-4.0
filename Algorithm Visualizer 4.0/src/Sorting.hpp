#pragma once
#include <ZUI.hpp>
#include <VisVec.hpp>

#include "OneDAlgo.hpp"

ONE_D_ALGORITHMS_FOR(Sorting) 
{
	SETUP_ALGO(7, "Bubble", "Insert", "Select", "Quick LL", "Quick LR", "Merge", "Heap");

	CREATE_ALGO(0, {
			int i, j, k;
			vec.addIndexTracker(&i, sf::Color::White);
			vec.addIndexTracker(&j, sf::Color::Green);
			vec.addIndexTracker(&k, sf::Color::Blue);

			vec.startRecording();

			for (i = 0; i < vec.size() - 1; i++) {
				for (j = 0; j < vec.size() - i - 1; j++) {
					k = j + 1;
					vec.saveFrame();

					if (vec[j + 1] < vec[j])
						vec.swap(j, j + 1);
				}
			}

			vec.stopRecording();

			vec.removeIndexTracker(&i);
			vec.removeIndexTracker(&j);
			vec.removeIndexTracker(&k);
		});

	CREATE_ALGO(1, {
		int i, j;
		vec.addIndexTracker(&i, sf::Color::White);
		vec.addIndexTracker(&j, sf::Color::Green);

		vec.startRecording();

		for (i = 1; i < vec.size(); i++) {
			int v = vec[i];
			j = i - 1;
			while (j >= 0 && vec[j] > v) {
				vec.assign(j + 1, vec[j]);
				j--;

				vec.saveFrame();
			}
			vec.assign(j + 1, v);
		}

		vec.stopRecording();

		vec.removeIndexTracker(&i);
		vec.removeIndexTracker(&j);
	});

	CREATE_ALGO(2, {
		int i, j, smallest_i;
		vec.addIndexTracker(&i, sf::Color::White);
		vec.addIndexTracker(&j, sf::Color::Green);
		vec.addIndexTracker(&smallest_i, sf::Color::Blue);

		vec.startRecording();

		for (i = 0; i < vec.size() - 1; i++) {
			smallest_i = i;
			for (j = i + 1; j < vec.size(); j++) {
				if (vec[j] < vec[smallest_i])
					smallest_i = j;

				vec.saveFrame();
			}
			vec.swap(i, smallest_i);
		}

		vec.stopRecording();

		vec.removeIndexTracker(&i);
		vec.removeIndexTracker(&j);
		vec.removeIndexTracker(&smallest_i);
	});

	void quickLLRecursive(int left, int right)
	{
		if (left < right)
		{
			int i, swap_index;
			vec.addIndexTracker(&left, sf::Color::White);
			vec.addIndexTracker(&right, sf::Color::White);
			vec.addIndexTracker(&i, sf::Color::Green);
			vec.addIndexTracker(&swap_index, sf::Color::Blue);

			int pivot = vec[right]; // rightmost as pivot (can be anything) (chosen for easier writing)
			swap_index = left; // index to swap values smaller than pivot

			for (i = left; i < right; i++) {  // from left to right if anything is smaller swap and place it before pivot
				if (vec[i] < pivot)
					vec.swap(i, swap_index++);
				vec.saveFrame();
			}

			vec.swap(swap_index, right); // swap pivot so that further recursions dont use the same pivot

			vec.removeIndexTracker(&left);
			vec.removeIndexTracker(&right);
			vec.removeIndexTracker(&i);
			vec.removeIndexTracker(&swap_index);

			quickLLRecursive(left, swap_index - 1);
			quickLLRecursive(swap_index + 1, right);
		}
	}
	CREATE_ALGO(3, {
		vec.startRecording();

		quickLLRecursive(0, vec.size() - 1);

		vec.stopRecording();
	});

	void quickLRRecursive(int left, int right)
	{
		if (left < right) {
			int l = left + 1, r = right;
			int pivot = vec[left];

			vec.addIndexTracker(&left, sf::Color::White);
			vec.addIndexTracker(&right, sf::Color::White);
			vec.addIndexTracker(&l, sf::Color::Green);
			vec.addIndexTracker(&r, sf::Color::Blue);

			while (l < r) {
				bool lc, rc;
				while ((lc = (l < vec.size() && vec[l] < pivot)) | (rc = (r >= 0 && vec[r] > pivot))) { // inc left if should, dec right if should
					if (lc)l++;
					if (rc)r--;
					vec.saveFrame();
				}

				if (l < r)vec.swap(l, r); // swap left and right
			}
			if (vec[r] < vec[left]) // swap pivot with r only if value at r is less as all lesser values should be on the left side
				vec.swap(r, left);

			vec.removeIndexTracker(&left);
			vec.removeIndexTracker(&right);
			vec.removeIndexTracker(&l);
			vec.removeIndexTracker(&r);

			quickLRRecursive(left, r - 1);
			quickLRRecursive(r + 1, right);
		}
	}
	CREATE_ALGO(4, {
		vec.startRecording();

		quickLRRecursive(0, vec.size() - 1);

		vec.stopRecording();
	});

	void mergeSortRecursive(int left, int right)
	{
		if (left >= right)return;

		int mid = (right - left) / 2 + left;

		mergeSortRecursive(left, mid);
		mergeSortRecursive(mid + 1, right);

		int i = 0, j = 0, k = left, il, jl;

		vec.addIndexTracker(&left, sf::Color::White);
		vec.addIndexTracker(&right, sf::Color::White);
		vec.addIndexTracker(&k, sf::Color::Green);
		vec.addIndexTracker(&il, sf::Color::Blue);
		vec.addIndexTracker(&jl, sf::Color::Blue);

		std::vector<int> a(vec.begin() + left, vec.begin() + mid + 1);
		std::vector<int> b(vec.begin() + mid + 1, vec.begin() + right + 1);

		vec.saveFrame();

		while (i < a.size() && j < b.size()) {
			if (a[i] < b[j])
				vec.assign(k++, a[i++]);
			else
				vec.assign(k++, b[j++]);

			il = i + left;
			jl = j + mid + 1;

			vec.saveFrame();
		}
		while (i < a.size()) {
			vec.assign(k++, a[i++]);
			il = i + left;
			vec.saveFrame();
		}
		while (j < b.size()) {
			vec.assign(k++, b[j++]);
			jl = j + mid + 1;
			vec.saveFrame();
		}

		vec.removeIndexTracker(&left);
		vec.removeIndexTracker(&right);
		vec.removeIndexTracker(&k);
		vec.removeIndexTracker(&il);
		vec.removeIndexTracker(&jl);
	}
	CREATE_ALGO(5, {
		vec.startRecording();

		mergeSortRecursive(0, vec.size() - 1);

		vec.stopRecording();
	});

	void make_heap(int n, int i)
	{
		int largest = i;
		int left = 2 * i + 1;
		int right = 2 * i + 2;

		if (left < n && vec[left] > vec[largest])
			largest = left;
		if (right < n && vec[right] > vec[largest])
			largest = right;

		if (largest != i) {
			vec.setIndexColor(i, sf::Color::Green);
			vec.setIndexColor(left, sf::Color::Blue);
			vec.setIndexColor(right, sf::Color::Blue);
			vec.saveFrame();

			vec.swap(i, largest);

			make_heap(n, largest);
		}
	}
	CREATE_ALGO(6, {
		int i;

		vec.addIndexTracker(&i, sf::Color::White);

		vec.startRecording();

		for (i = vec.size() / 2 - 1; i >= 0; i--) {
			make_heap(vec.size(), i);
		}
		for (i = vec.size() - 1; i > 0; i--) {
			vec.swap(0, i);
			make_heap(i, 0);
		}

		vec.stopRecording();
	});
};


