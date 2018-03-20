void sng_shaveandahaircut(){
	int melody[] = {
	  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
	};
	// note durations: 4 = quarter note, 8 = eighth note, etc.:
	int noteDurations[] = {
	  4, 8, 8, 4, 4, 4, 4, 4
	};
	for (int thisNote = 0; thisNote < 8; thisNote++) {
		int noteDuration = musicspd / noteDurations[thisNote];
		NewTone(buzzerpin, melody[thisNote], noteDuration);
		int pauseBetweenNotes = noteDuration * 1.30;
		delay(pauseBetweenNotes);
		noNewTone(buzzerpin);
	}
}
