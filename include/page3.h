#ifndef PAGE3_H
#define PAGE3_H

#include <TFT_eSPI.h>
#include "arrow.h"    // 引入箭頭圖案的資料檔案
#include "city.h"     // 引入背景圖片的資料檔案

void showPage3(TFT_eSPI &tft, TFT_eSprite &background) {
    static int x = 20; // 定義箭頭初始的水平位置
    TFT_eSprite arrowSprite = TFT_eSprite(&tft); // 宣告一個 sprite 用於顯示箭頭
    TFT_eSprite txtSprite = TFT_eSprite(&tft);  // 宣告一個 sprite 用於顯示文字

    // 清空背景 sprite
    background.fillSprite(TFT_BLACK); // 使用黑色清空背景

    // 建立箭頭 sprite，大小為 96x96
    arrowSprite.createSprite(96, 96);

    // 建立文字 sprite，大小為 80x80
    txtSprite.createSprite(80, 80);
    txtSprite.setTextColor(TFT_WHITE, TFT_BLACK); // 設定文字顏色為白色，背景顏色為黑色

    // 將背景圖片繪製到背景 sprite
    background.pushImage(0, 0, 320, 170, city);

    // 清空文字 sprite 並填充為黑色背景
    txtSprite.fillSprite(TFT_BLACK);
    // 在文字 sprite 上繪製數字，顯示當前的箭頭位置
    txtSprite.drawString(String(x), 0, 0, 6);

    // 將箭頭圖案繪製到箭頭 sprite
    arrowSprite.pushImage(0, 0, 96, 96, arrow);
    // 將箭頭 sprite 推送到背景 sprite，位置由 x (水平座標) 和 40 (垂直座標) 決定
    arrowSprite.pushToSprite(&background, x, 40, TFT_BLACK);
    // 將文字 sprite 推送到背景 sprite，顯示於右上角
    txtSprite.pushToSprite(&background, 200, 30, TFT_BLACK);

    // 將背景 sprite 推送到螢幕上，完成一幀的顯示
    background.pushSprite(0, 0);

    // 更新箭頭的水平位置，每次迴圈向右移動一個像素
    x++;

    // 如果箭頭超出螢幕範圍，則將其重置到左側 (x = -100)
    if (x > 330) {
        x = -100;
    }

    arrowSprite.deleteSprite(); // 刪除箭頭 sprite 以釋放記憶體
    txtSprite.deleteSprite(); // 刪除文字 sprite 以釋放記憶體
}

#endif // PAGE3_H