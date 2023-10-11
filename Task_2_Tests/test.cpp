#include "pch.h"
#include "../Task_2/DynamicArray.h"

TEST(Insert, InsertInt)
{
  DynamicArray<int> arr;
  
  for (int i = 0; i < 8; ++i)
  {
    arr.Insert(i);
  }

  for (int i = 0; i < 8; ++i)
  {
    ASSERT_EQ(arr[i], i);
  }
}

TEST(Insert, InsertString)
{
  DynamicArray<std::string> arr;
  std::string source_arr[] = { "foo",  "bar", "buz", "sakkijarvenpolkka"};
  
  for (const auto& i : source_arr)
  {
    arr.Insert(i);
  }

  for (int i = 0; i < 4; ++i)
  {
    ASSERT_EQ(arr[i], source_arr[i]);
  }
}

TEST(Insert, InsertIntByIndex)
{
  DynamicArray<int> arr;

  for (int i = 0; i < 8; ++i)
  {
    arr.Insert(0, i);
  }

  for (int i = 0; i < 8; ++i)
  {
    ASSERT_EQ(arr[i], 7-i);
  }
}

TEST(Insert, InsertStringByIndex)
{
  DynamicArray<std::string> arr;
  const std::string target_arr[] = { "foo", "buz", "bar"};
  
  arr.Insert("foo");
  arr.Insert("bar");
  arr.Insert(1, "buz");

  for (int i = 0; i < 3; ++i)
  {
    ASSERT_EQ(arr[i], target_arr[i]);
  }
}

TEST(Remove, RemoveInt)
{
  DynamicArray<int> arr;
  const int target_arr[] = { 0, 1, 2, 3, 6, 7};
  
  for (int i = 0; i < 8; ++i)
  {
    arr.Insert(i);
  }

  arr.Remove(4);
  arr.Remove(4);
  
  for (int i = 0; i < 6; ++i)
  {
    ASSERT_EQ(arr[i], target_arr[i]);
  }
}

TEST(Remove, RemoveString)
{
  DynamicArray<std::string> arr;
  const std::string source_arr[] = { "foo", "buz", "bar"};
  const std::string target_arr[] = { "foo", "bar"};
  
  for (const auto& i : source_arr)
  {
    arr.Insert(i);
  }

  arr.Remove(1);
  
  for (int i = 0; i < 2; ++i)
  {
    ASSERT_EQ(arr[i], target_arr[i]);
  }
}

TEST(Size, AddSizeChange)
{
  DynamicArray<std::string> arr;
  
  for (int i = 0; i < 12; ++i)
  {
    arr.Insert("42");
  }
  
  const int arr_size_old = arr.size();

  for (int j = 0; j < 5; ++j)
  {
    arr.Insert("foo");
  }
  
  ASSERT_GT(arr.size(), arr_size_old);
}

TEST(Size, RemoveSizeChange)
{
  DynamicArray<std::string> arr;
  
  for (int i = 0; i < 12; ++i)
  {
    arr.Insert("42");
  }
  
  const int arr_size_old = arr.size();

  for (int j = 0; j < 5; ++j)
  {
    arr.Remove(0);
  }
  
  ASSERT_LT(arr.size(), arr_size_old);
}

TEST(Iterator, TestForwardIteratorGetInt)
{
  DynamicArray<int> arr;

  for (int i = 0; i < 8; ++i)
  {
    arr.Insert(i+1);
  }

  int prev_value = 0;
  for (auto it = arr.iterator(); it.hasNext(); it.next())
  {
    ASSERT_LT(prev_value, it.get());
    prev_value = it.get();
  }
}

TEST(Iterator, TestForwardIteratorGetString)
{
  DynamicArray<std::string> arr;
  std::string source_arr[] = { "foo",  "bar", "buz"};
  
  for (const auto& i : source_arr)
  {
    arr.Insert(i);
  }

  int i = 0;
  for (auto it = arr.iterator(); it.hasNext(); it.next())
  {
    ASSERT_EQ(source_arr[i], it.get());
    i++;
  }
}

TEST(Iterator, TestReversedIteratorGetInt)
{
  DynamicArray<int> arr;

  for (int i = 0; i < 8; ++i)
  {
    arr.Insert(i+1);
  }

  int prev_value = 9;
  for (auto it = arr.reversedIterator(); it.hasNext(); it.next())
  {
    ASSERT_GT(prev_value, it.get());
    prev_value = it.get();
  }
}

TEST(Iterator, TestReversedIteratorGetString)
{
  DynamicArray<std::string> arr;
  std::string source_arr[] = { "foo",  "bar", "buz"};
  
  for (const auto& i : source_arr)
  {
    arr.Insert(i);
  }

  int i = 0;
  for (auto it = arr.reversedIterator(); it.hasNext(); it.next())
  {
    ASSERT_EQ(source_arr[2-i], it.get());
    i++;
  }
}

TEST(Iterator, TestForwardIteratorSetInt)
{
  DynamicArray<int> arr;
  
  for (int i = 0; i < 8; ++i)
  {
    arr.Insert(i);
  }

  for (auto it = arr.iterator(); it.hasNext(); it.next())
  {
    it.set(42);
  }
  
  for (auto it = arr.iterator(); it.hasNext(); it.next())
  {
    ASSERT_EQ(42, it.get());
  }
}

TEST(Iterator, TestForwardIteratorSetString)
{
  DynamicArray<std::string> arr;
  std::string source_arr[] = { "foo",  "bar", "buz"};
  
  for (const auto& i : source_arr)
  {
    arr.Insert(i);
  }

  for (auto it = arr.iterator(); it.hasNext(); it.next())
  {
    it.set("42");
  }
  
  for (auto it = arr.iterator(); it.hasNext(); it.next())
  {
    ASSERT_EQ("42", it.get());
  }
}