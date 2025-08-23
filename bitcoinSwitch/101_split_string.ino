int splitString(String input, char delimiter, String parts[], int maxParts) {
  int partIndex = 0;
  int startIndex = 0;
  int delimIndex;
  while ((delimIndex = input.indexOf(delimiter, startIndex)) != -1 && partIndex < maxParts) {
    parts[partIndex++] = input.substring(startIndex, delimIndex);
    startIndex = delimIndex + 1;
  }
  // Add the last part
  if (partIndex < maxParts) {
    parts[partIndex++] = input.substring(startIndex);
  }
  return partIndex; // number of parts found
}
