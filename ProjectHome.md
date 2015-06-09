A C++-based software allows to recognize simple HCI gestures to control the linux open source computer game SuperTux, using one motion sensor typically placed on the forearm (or two, on the left and right forearms).

First, training gestures are recorded for each of the controls of the game (e.g. move left/right, jump).
When several sensors are used the software automatically detects which sensor is relevant for the training gesture.

The recognition chain consists of energy-based segmentation (see above).
Two feature extraction approaches offer different latency-accuracy trade-offs.
In the first approach, features are computed on the acceleration signal on a window aligned to the start of the segment, with a length specified by the user (typically about 100ms long).
This assumes that gestures are different in their starting phase, and allows to achieve lower-latency than the second approach.
In the second approach, features are computed on the entire segment (i.e. from start to end of gesture).
Therefore classification can only be performed when the gesture is completed, which translates into higher latency and potentially higher accuracy.
The features that are computed include among others mean, variance, energy distribution, zero-crossing or mean-crossing rate of the sensor signal.
The sensor signal can be the the acceleration along a specific axis, the acceleration magnitude, or the rate of turn.
The user selects by means of a configuration file which features are used.
Eventually, the features are classified with a nearest class centroid (NCC) classifier, which is a simple, low-complexity classifier well suited for embedded implementation.
To avoid matching outliers a ``null-class rejection'' rejects the gesture if the features are located further away from the class centroid than a threshold distance.
Eventually the detected gesture is sent to the modified SuperTux game as a UDP packet, thus essentially simulating a key press.

The main educational aspects are: i) introduction to the topic of human-computer interaction; ii) obtaining low-latency gesture classification; iii) understanding what are meaningful features to classify the desired gestures.