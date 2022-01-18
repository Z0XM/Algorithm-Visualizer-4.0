#include <VisVec.hpp>

int main(int argc, char** argv)
{
    VisVec vec;
    for (int i = 2; i < argc; i++)
        vec.push_back(std::stoi(argv[i]));

	////////////////////////////////////////////////
	/// Type Your Code Below ///////////////////////
	////////////////////////////////////////////////

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

	////////////////////////////////////////////////
	/// Type Your Code Above ///////////////////////
	////////////////////////////////////////////////

    vec.writeAnimationDataToFile("data_output.txt");

    return 0;
}