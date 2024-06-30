import 'dart:math';

import 'package:flutter/material.dart';

class my_app_state extends ChangeNotifier {

  int data1 = 0;
  Random random = new Random();

  double valueChange() {
    data1 = random.nextInt(100);
    notifyListeners();
    return data1 as double;
  }
}