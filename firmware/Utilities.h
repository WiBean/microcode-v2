/**
 ************
 * UTILITY
 ************
 */

template <typename T>
void printArray(uint16_t numel, T const*const array)
{
  Serial.print('[');
  for(uint16_t k=0;k<numel-1;++k) {
    Serial.print(array[k]);
    Serial.print(", ");
  }
  Serial.print(array[numel-1]);
  Serial.println("]");
};
