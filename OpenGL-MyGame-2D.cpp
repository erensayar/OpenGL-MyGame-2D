//
//<===========================================================================>
//
// Proje: OpenGL API Kullanarak Oyun Geliştirme
// Yazar: Eren Sayar
// Haziran 2020 - İstanbul
//
/*<===========================================================================>
Açıklama: OpenGL Grafik Programlama API’si kullanarak geliştirilen oyunda; ekranda
yatay eksende hareket eden helikopterler bulunmaktadır. Rastgele hızlara ve renklere
sahip olan helikopterler ekranın sağ ve solundan çıkabilmektedir. Ekranın alt kısmında
bulunan uçak ise helikopterlere çarpmadan ekranın diğer tarafına ulaşması istenmektedir.
Uçak figürünün herhangi bir noktasının,helikopterlerden herhangi birisi ile noktasal
olarak teması çarpışma olarak kabul edilecektir. Pencerenin Sonuna ulaşılmasıyla oyun
başarıyla tamamlanmış olacaktır. Her tamamlamada 10 puan alınacaktır. Skor arttıkça
oyun da helikopterlerin hızlanmasıyla zorlaşacaktır. Oyuncunun 3 çarpışma hakkı bulunur.
<============================================================================>*/
/*Versiyon 1.6 Not: ScoreBoard ve Giriş Ekranı Animasyonu Eklendi
<============================================================================>*/
//
// Uçak İçin Kontroller
// Sağ ok - Sağa İlerleme
// Sol ok - Sola İlerleme
// Üst ok - Yukarı İlerleme
// Alt ok - Aşağı İlerleme
//
//<===========================================================================>
// Kütüphaneler projeye dahil edilir.
#include <GL/freeglut.h>  // Grafik işlemleri, pencere açma, girdiler alabilmek vb. için	 	  
#include <iostream>	 	  // cout	 	  
#include <random>	 	  // Random Atamalar İçin
#include <thread>    	  // thread
#include <string>	 	  // String İşlemleri İçin
#include <fstream>	 	  // Giriş/Çıkış İşlemleri İçin
#include <vector> 	 	  // ArrayList karşılığı C++'da vektördür. Skorların tutulması için vektör kullanıldı
using namespace std;

// Global Değişkenler Ve Sabitler
static const int FPS = 60; // Kare hızı

// Pencere Boyutu
const int window_height = 640;
const int window_width = 480;

/* Taşmaları önlemek için nesne boyutunun yarısı
kadar ekleme ve çıkarma yapıldı. 50/2=25 (merkez) */
const int most_up = 615;	// 640-25=615  
const int most_down = 25;	// 0+25=25	  
const int most_rigth = 455;	// 480-25=455  
const int most_left = 25;	// 0+25=25     

// Uçağın varsayılan x ve y koordinatı
// Random olarak sonradan ayarlanacaktır
float plane_x = 0.0;
float plane_y = 0.0;

// Helikopterlerin Varsayılan X Koordinatı
// Başlangıçta en solun 50px solundan ve en sağın 50px Sağından
float heli1_x = most_left - 50;
float heli2_x = most_rigth + 50;
float heli3_x = most_left - 50;
float heli4_x = most_rigth + 50;

/*
Y Koordinatları arasında 115 px boşluk vardır. Bunun sebebi nesnenin en
sol altından diğer nesnenin en sol altı arasındaki farktır. Bu durum iki
helikopter arasında 65px (115-50=65) alan olduğu anlamına gelmektedir.
65px olmasının sebebi ise; 50px uçak alanı 15px de saklanma payıdır.
*/
//Helikopterlerin Y Koordinatları Hep Aynı Kalacaktır
float heli1_y = 245.0; // Y Ekseninde 240. piksel sonrasında   5. Pixel Düzeyinde
float heli2_y = 360.0; // Y Ekseninde 240. piksel sonrasında 120. Pixel Düzeyinde
float heli3_y = 475.0; // Y Ekseninde 240. piksel sonrasında 235. Pixel Düzeyinde
float heli4_y = 590.0; // Y Ekseninde 240. piksel sonrasında 350. Pixel Düzeyinde

// Varsayılan uçak hareket miktarı(pixel)
int plane_movement_pixel_size = 10;

// Varsayılan helikopter hareket miktarı(pixel)
// Random Hız Tayini Ve Hareket İçin Gerekli
int heli1_movement_pixel_size = 5;
int heli2_movement_pixel_size = 5;
int heli3_movement_pixel_size = 5;
int heli4_movement_pixel_size = 5;

// Helikopterlerin Varsayılan Renk Değerleri
float heli1_color_R = 1.0;
float heli1_color_G = 0.0;
float heli1_color_B = 0.0;

float heli2_color_R = 0.0;
float heli2_color_G = 1.0;
float heli2_color_B = 0.0;

float heli3_color_R = 1.0;
float heli3_color_G = 0.0;
float heli3_color_B = 1.0;

float heli4_color_R = 0.5;
float heli4_color_G = 0.0;
float heli4_color_B = 1.0;

// Helikopterlerin varsayılan yön değerleri
// 0 ->Soldan Sağa | 1-> Sağdan Sola
// Random yön tayini için gerekli
bool heli1_DescriptionStatus = 0;
bool heli2_DescriptionStatus = 1;
bool heli3_DescriptionStatus = 0;
bool heli4_DescriptionStatus = 1;

// Varsayılan hız min ve max değerler
//Random olarak değişecektir
int minSpeed = 2;
int maxSpeed = 4;

// Varsayılan skor, can ve zorluk değerleri
int score = 0;
int difficulty = 1;
int health = 3;

// Ekrana yazı yazmak için gerekli değişkenler
// Font değişkeni ile yazı tipi ve boyutu belirlendi
// Varsayılan Can ve Puan Değerleri Belirlendi
const int font = (int)GLUT_BITMAP_9_BY_15;
const int font2 = (int)GLUT_BITMAP_TIMES_ROMAN_24;
char scoreCharString[2] = "0";
char healthCharString[2] = "3";

// Oyuncu ismi belirlendi
// Skor tablosunun tutulacağı dosya için değişkenler tanımlandı
char playerNameCharString[10] = "";
ifstream readData("scores.txt");
ofstream writeData;

int tempUserNameIndis = 0;

// Skor butonunun konumu
float scoreButton_x = 180;
float scoreButton_y = 50;

// Giriş ekranındaki nesne için
float animation_quad_x = 215.0;
float animation_quad_y = 450.0;
float animation_quad2_x = 265.0;
float animation_quad2_y = 500.0;
int animation_quad_status;
float animation_quad_movement_pixel_size = 1.0;

/*
(İptal)

// Skorları tutan vektörler(Arrayler)
vector<string> arrayOfName;
vector<string> arrayOfScore;

// Geçici değer tutucular
char arrayOfNameChar[20];
char arrayOfScoreChar[20];
*/

//<===========================================================================>
// Ekrana yazdırılan yazı render edilir
/*Bu yapı ile yazının yerleştirileceği koordinat ve font tipi alınır ayrıca char
tipindeki karakterler döngü ile bellekte gezinerek göstericiler aracılığıyla alınır
bitmapteki karşılıkları da glutBitmapCharacter fonksiyonu ile belirlenir ekrana bastırılır*/
void renderBitmapString(float x, float y, void *font, const char *string) {
	const char *c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}
//<===========================================================================>
// Oyun giriş ekranı
void displayHome() {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Giriş Ekran Animasyonu
	glColor3f(0.76, 0.0, 0.19);
	glPushMatrix();
	glTranslatef(animation_quad_x, animation_quad_y, 0.0);
	glRectf(-25, -25, 25, 25);
	glPopMatrix();

	glColor3f(0.204, 0.255, 0.255);
	glPushMatrix();
	glTranslatef(animation_quad2_x, animation_quad2_y, 0.0);
	glRectf(-25, -25, 25, 25);
	glPopMatrix();

	// Ekrana yazdırma işlemi
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(190, 360, (void *)font2, "Wellcome");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(174, 290, (void *)font, "Enter User Name");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(120, 270, (void *)font, "Use Character From Keyboard");
	glColor3f(1.0, 0.0, 0.0);
	renderBitmapString(174, 245, (void *)font, playerNameCharString);
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(178, 240, (void *)font, "_____________");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(150, 150, (void *)font, "Press Enter For Play");

	// Skor Ekranı Butonu Yazısı Ve Buton Dörtgeni
	glColor3f(0.76, 0.0, 0.19);
	glBegin(GL_QUADS);
	glVertex2f(0 + scoreButton_x, 0 + scoreButton_y);
	glVertex2f(120 + scoreButton_x, 0 + scoreButton_y);
	glVertex2f(120 + scoreButton_x, 30 + scoreButton_y);
	glVertex2f(0 + scoreButton_x, 30 + scoreButton_y);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	renderBitmapString(185, 58, (void *)font2, "ScoreBoard");

	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(135, 30, (void *)font, "Use Mouse For ScoreBoard");

	glFlush();
	glutSwapBuffers();
}
//<===========================================================================>
// Oyun ekranı 
void displayGame() {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Skor ve kalan hak yazıları sağ üste yazdırılır.
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(290, 620, (void *)font, "Puan: ");

	glColor3f(0.76, 0.0, 0.19);
	renderBitmapString(340, 620, (void *)font, scoreCharString);

	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(370, 620, (void *)font, "Kalan Hak: ");

	glColor3f(0.76, 0.0, 0.19);
	renderBitmapString(464, 620, (void *)font, healthCharString);

	//Uçak
	glColor3f(0.0, 0.0, 1.0);
	glPushMatrix();
	glTranslatef(plane_x, plane_y, 0.0);
	glRectf(-25, -25, 25, 25);
	glPopMatrix();

	//Helikopter 1
	glColor3f(heli1_color_R, heli1_color_G, heli1_color_B);
	glPushMatrix();
	glTranslatef(heli1_x, heli1_y, 0.0);
	glRectf(-25, -25, 25, 25);
	glPopMatrix();

	//Helikopter 2
	glColor3f(heli2_color_R, heli2_color_G, heli2_color_B);
	glPushMatrix();
	glTranslatef(heli2_x, heli2_y, 0.0);
	glRectf(-25, -25, 25, 25);
	glPopMatrix();

	//Helikopter 3
	glColor3f(heli3_color_R, heli3_color_G, heli3_color_B);
	glPushMatrix();
	glTranslatef(heli3_x, heli3_y, 0.0);
	glRectf(-25, -25, 25, 25);
	glPopMatrix();

	//Helikopter 4
	glColor3f(heli4_color_R, heli4_color_G, heli4_color_B);
	glPushMatrix();
	glTranslatef(heli4_x, heli4_y, 0.0);
	glRectf(-25, -25, 25, 25);
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}
//<===========================================================================>
// Tuşlarla Dinamik Hareket İçin Fonksiyon.
void specialKeyGame(int key, int x, int y) {

	switch (key) {

	case GLUT_KEY_UP:
		if (plane_y < most_up) plane_y += plane_movement_pixel_size;    // Yukarı İlerleme Sağlar Ve Yukarı Taşmaları Önler
		break;

	case GLUT_KEY_DOWN:
		if (plane_y > most_down) plane_y -= plane_movement_pixel_size;  // Aşağı İlerleme Sağlar Ve Aşağı Taşmaları Önler
		break;

	case GLUT_KEY_LEFT:
		if (plane_x > most_left) plane_x -= plane_movement_pixel_size;  // Sola İlerleme Sağlar Ve Sola Taşmaları Önler
		break;

	case GLUT_KEY_RIGHT:
		if (plane_x < most_rigth) plane_x += plane_movement_pixel_size; // Sağa İlerleme Sağlar Ve Sağa Taşmaları Önler
		break;
	}
	glutPostRedisplay(); // Görüntü güncellenmesi
}
//<===========================================================================>
/*keyboardFuncHome()un pasifize edilmesi için tanımlanmştır */
void keyboardFuncExtra(unsigned char key, int x, int y) {
	cout << "Just Use Default Input Arguments";
}
//<===========================================================================>
/*Giriş Ekranında klavye işlemleri burdan yapılır. Skor tablosunu depolamak
için dosya giriş/çıkış işlemleri ile metin dosyasına veriler yazılır ve okunur
keyboardfunc olarak da işlevsiz bir keyboard fuction ayarlanır ki diğer ekranda
bu keyboardFuncHome()'un tetiklenmemesi sağlanır böylece kullanıcı ismi bozulmadan
alınır*/
void keyboardFuncHome(unsigned char key, int x, int y) {
	cout << key;
	playerNameCharString[tempUserNameIndis] = key;

	if (key == 13) {
		glutDisplayFunc(displayGame);
		glutSpecialFunc(specialKeyGame);
		glutKeyboardFunc(keyboardFuncExtra);
	}

	tempUserNameIndis++;
}
//<===========================================================================>
// Oyun bitiş ekranı
void displayFinish() {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Ekrana yazdırma işlemi
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(185, 400, (void *)font2, "Game Over");

	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(145, 300, (void *)font, "Press Enter For Replay");

	glFlush();
	glutSwapBuffers();
}
//<===========================================================================>
/*Bu fonksiyonun kullanılma sebebi game over ekranındayken specialKey() fonksyonunun
devamlı etkin olmasıdır bu durumun önüne geçmek için gameover ekranındayken specialkey2
fonksiyonu kullanıma alınır. specialKey() böylece pasifize edilir. specialKey'i aktive
etmek ve oyunu başlatmak içinse enter tuşuna basmak yeterlidir*/
void specialKeyExtra(int key, int x, int y) {
	switch (key) {
	default:
		cout << endl << "Just Use Enter.";
		break;
	}
}
//<===========================================================================>
// Game Over Ekranında Enter Tuşu Yakalanır
// Enter Tuşu İle Değerler Varsayılan Olarak Sıfırlanır
// Oyun Yeniden Başlar
void keyboardFuncFinish(unsigned char key, int x, int y) {

	// GLUT Enter tuşunu glutKeyboardFunc() Callback fonksiyonu için 13 olarak tanımlamıştır
	if (key == 13) {
		// Değerler varsayılan olarak yeniden ayarlandı
		score = 0;
		health = 3;
		difficulty = 1;
		tempUserNameIndis = 0;
		for (int i = 0; i < 10; i++) {
			playerNameCharString[i] = NULL;
		}
		plane_movement_pixel_size = 10;
		glutDisplayFunc(displayHome);
		glutKeyboardFunc(keyboardFuncHome);

		// Puan ve can değerlerinin ekranda da güncellenmesi sağlanır.
		string scoreString = to_string(score);
		for (int i = 0; i < scoreString.length(); i++) {
			scoreCharString[i] = scoreString.at(i);
		}

		string healthString = to_string(health);
		for (int i = 0; i < healthString.length(); i++) {
			healthCharString[i] = healthString.at(i);
		}

	}
}
//<===========================================================================>
// Score tablosu
void displayScore() {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(190, 550, (void *)font2, "SCORES");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(150, 520, (void *)font, "(Last Added On Top)");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(145, 490, (void *)font, "(This Data's Static)");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(102, 470, (void *)font, "(This Board A Concept Design)");

	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(180, 400, (void *)font2, "Eren");
	glColor3f(0.76, 0.0, 0.19);
	renderBitmapString(280, 400, (void *)font2, "30");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(180, 370, (void *)font2, "Pelin");
	glColor3f(0.76, 0.0, 0.19);
	renderBitmapString(280, 370, (void *)font2, "10");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(180, 340, (void *)font2, "Helin");
	glColor3f(0.76, 0.0, 0.19);
	renderBitmapString(280, 340, (void *)font2, "60");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(180, 310, (void *)font2, "Furkan");
	glColor3f(0.76, 0.0, 0.19);
	renderBitmapString(280, 310, (void *)font2, "30");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(180, 280, (void *)font2, "Cansu");
	glColor3f(0.76, 0.0, 0.19);
	renderBitmapString(280, 280, (void *)font2, "0");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(180, 250, (void *)font2, "Yaren");
	glColor3f(0.76, 0.0, 0.19);
	renderBitmapString(280, 250, (void *)font2, "0");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(180, 220, (void *)font2, "Jason");
	glColor3f(0.76, 0.0, 0.19);
	renderBitmapString(280, 220, (void *)font2, "10");
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(180, 190, (void *)font2, "Jennifer");
	glColor3f(0.76, 0.0, 0.19);
	renderBitmapString(280, 190, (void *)font2, "20");


	// Çıkış Butonu
	// Değişken isimleri değiştirilmedi. Konumlar aynı olacak
	glColor3f(0.76, 0.0, 0.19);
	glBegin(GL_QUADS);
	glVertex2f(0 + scoreButton_x, 0 + scoreButton_y);
	glVertex2f(120 + scoreButton_x, 0 + scoreButton_y);
	glVertex2f(120 + scoreButton_x, 30 + scoreButton_y);
	glVertex2f(0 + scoreButton_x, 30 + scoreButton_y);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	renderBitmapString(210, 58, (void *)font2, "BACK");

	glFlush();
	glutSwapBuffers();
}
//<===========================================================================>
// Butonun başlangıç noktası: x -> 185 | y -> 50
// Uzunluk olarak; x -> 120 | y -> 30
// Ayarlanan butonun köşe koordinatları şu o halde şekildedir:
// V1(185,50), V2(290,50), V3(290,100), V4(185,100) 
// Ancak Y ekseni için alttan yukarıya artış söz konusu değil(Mouse İçin)
// Dolayısıyla 50 ve 80 değil 590 ve 560 aralığı alınacaktır 
// Bu aralıklarda mouse'un sol kliğinin basılması istenir
//-------------------------------------------------------
// Fonksiyonu kullanabilmek için bildirim yapılıyor:
//-------------------------------------------------------
void mouseScoreBoard(int button, int state, int x, int y);
//-------------------------------------------------------
void mouseHome(int button, int state, int x, int y) {
	if (
		button == GLUT_LEFT_BUTTON &&
		state == GLUT_DOWN &&
		x <= 305 && x >= 185 &&
		y <= 590 && y >= 560
		) {
		glutDisplayFunc(displayScore);
		glutKeyboardFunc(keyboardFuncExtra);
		glutMouseFunc(mouseScoreBoard);
	}
}
//<===========================================================================>
void mouseScoreBoard(int button, int state, int x, int y) {
	if (
		button == GLUT_LEFT_BUTTON &&
		state == GLUT_DOWN &&
		x <= 305 && x >= 185 &&
		y <= 590 && y >= 560
		) {
		glutDisplayFunc(displayHome);
		glutKeyboardFunc(keyboardFuncHome);
		glutMouseFunc(mouseHome);
	}
}
//<===========================================================================>
// Uçak x ekseninde 0-480, y ekseninde 0-240 piksel aralığında random olarak konumlandırılacaktır.
void setPlaneRandomLocation() {

	random_device rand_dev;
	mt19937 generator(rand_dev());

	uniform_int_distribution<int> randomPlaneX(most_left, 430);// 0-430 Aralığında Random | 480-50=430 -> Uçak 50x50 alan kaplamakta o yüzden 50 çıkarıldı
	plane_x = randomPlaneX(generator); // Uçak X koordinatı random olarak atandı

	uniform_int_distribution<int> randomPlaneY(most_down, 190); // 0-190 Aralığında Random | 240-50=190 -> Uçak 50x50 alan kaplamakta o yüzden 50 çıkarıldı
	plane_y = randomPlaneY(generator); // Uçak Y koordinatı random olarak atandı

	cout << endl << "Plane X Location: " << plane_x << endl; // Terminalde Uçağın X Koordinatı Görünür
	cout << "Plane Y Location: " << plane_y << endl; // Terminalde Uçağın Y Koordinatı Görünür
}
//<===========================================================================>
int randomDescription() {

	random_device rand_dev;
	mt19937 generator(rand_dev());
	uniform_int_distribution<int> randomDescription(0, 1);
	bool description = ((bool)(randomDescription(generator)));

	return description;
}
//<===========================================================================>
void randomColor(int heliNumber) {

	random_device rand_dev;
	mt19937 generator(rand_dev());
	uniform_int_distribution<int> randomColorValue(0, 100);

	if (heliNumber == 1) {
		heli1_color_R = ((float)randomColorValue(generator) / 100);
		heli1_color_G = ((float)randomColorValue(generator) / 100);
		heli1_color_B = ((float)randomColorValue(generator) / 100);
	}
	else if (heliNumber == 2) {
		heli2_color_R = ((float)randomColorValue(generator) / 100);
		heli2_color_G = ((float)randomColorValue(generator) / 100);
		heli2_color_B = ((float)randomColorValue(generator) / 100);
	}
	else if (heliNumber == 3) {
		heli3_color_R = ((float)randomColorValue(generator) / 100);
		heli3_color_G = ((float)randomColorValue(generator) / 100);
		heli3_color_B = ((float)randomColorValue(generator) / 100);
	}

	else if (heliNumber == 4) {
		heli4_color_R = ((float)randomColorValue(generator) / 100);
		heli4_color_G = ((float)randomColorValue(generator) / 100);
		heli4_color_B = ((float)randomColorValue(generator) / 100);
	}
}
//<===========================================================================>
// Helikopter Random Hız
// Zorluğa göre belirli aralıklarda random hız ayarlanmaktadır.
int randomSpeed(int difficulty) {

	switch (difficulty) {

	case 1:
		minSpeed = 2;
		maxSpeed = 4;
		break;

	case 2:
		minSpeed = 4;
		maxSpeed = 7;
		break;
	case 3:
		minSpeed = 4;
		maxSpeed = 10;
		break;
	case 4:
		minSpeed = 10;
		maxSpeed = 14;
		break;
	default:
		minSpeed = 10;
		maxSpeed = 14;
		break;
	}
	random_device rand_dev;
	mt19937 generator(rand_dev());
	uniform_int_distribution<int> randomSpeed(minSpeed, maxSpeed);
	int i = randomSpeed(generator);

	return i;
}
//<===========================================================================>
// Çarpışma Ve Oyunu Başarıyla Bitirme Kontrolü
// Sürekli bir kontrol olması için sonsuz döngü kullanılmıştır
/* Bu döngünün de glut dögüsünü kilitlememesi içinse bu fonksiyon
thread yapısı sayesinde paralel bir işlem olarak kullanılmaktadır */
void impactControll() {

	while (true) {

		//Çarpışma Kontrolü
		if (
			abs(plane_x - heli1_x) <= 50 && abs(plane_y - heli1_y) <= 50 ||
			abs(plane_x - heli2_x) <= 50 && abs(plane_y - heli2_y) <= 50 ||
			abs(plane_x - heli3_x) <= 50 && abs(plane_y - heli3_y) <= 50 ||
			abs(plane_x - heli4_x) <= 50 && abs(plane_y - heli4_y) <= 50
			) {

			Beep(523, 500); // 523 hertz (C5) 500 milisaniye
			health -= 1; // Can çarpışma sebebiyle düşürüldü

			//------------------------------------------------
			/* int tipindeki health verisi string tipine dönüştürüldü sonrasında ise char
			dizisine atandı. Bu işlemin sebebi rasterBitmapString fonksiyonundan kaynaklı,
			amaç int veri tipini sıralı olarak dönüştürerek bastırma fonksiyonuna
			göstericiler ile verebilmektir.*/
			string healthString = to_string(health);
			for (int i = 0; i < healthString.length(); i++) {
				healthCharString[i] = healthString.at(i);
			}
			//------------------------------------------------

			cout << endl << "!!! Impact !!!" << endl << "Health: " << health << endl;

			if (health == 0) {
				cout << endl << "|||||||||||||||||||||||||||";
				cout << endl << "|||||||| Game Over ||||||||";
				cout << endl << "|||||||||||||||||||||||||||" << endl;


				// Skor ve oyuncu ismi kaydedilir
				try {
					// Veri Yazma İşlemi
					writeData.open("scores.txt", ios::out | ios::app);

					if (writeData.is_open()) {

						int i = 1;
						while (playerNameCharString[i] != '\0') {
							writeData << playerNameCharString[i - 1];
							i++;
						}
						writeData << ">" << score << "|";
						writeData.close();
					}
				}
				catch (int errorCode) {
					cout << "Error Code: " << errorCode;
				}


				/* Oyunun bitmesi sonucu bitiş ekranına özgü nesneler ve tuş
				kombinasyonlarını içeren fonksiyonlar callback metodlarıyla
				çağrılarak bitiş ekranı oluşturulmuştur */
				glutDisplayFunc(displayFinish);
				glutKeyboardFunc(keyboardFuncFinish);
				glutSpecialFunc(specialKeyExtra);
			}

			setPlaneRandomLocation();
		}

		//Bölüm Geçme Kontrolü
		if (plane_y >= most_up) {
			Beep(400, 1000); // 400 hz 1000 milisaniye
			difficulty += 1; // Bölüm bitirilince zorluk derecesi 1 artırıldı
			score += 10; // Bölüm bitirilince skor 10 artırıldı

			//------------------------------------------------
			/* int tipindeki score verisi string tipine dönüştürüldü sonrasında ise char
			dizisine atandı. Bu işlemin sebebi rasterBitmapString fonksiyonundan kaynaklı,
			amaç int veri tipini sıralı olarak dönüştürerek bastırma fonksiyonuna
			göstericiler ile verebilmektir.*/
			string scoreString = to_string(score);
			for (int i = 0; i < scoreString.length(); i++) {
				scoreCharString[i] = scoreString.at(i);
			}
			//------------------------------------------------

			// Dengeli bir zorluk olması için uçak hareket hızı da artacaktır
			plane_movement_pixel_size += 2;

			if (score >= 30)
				plane_movement_pixel_size += 8;

			cout << endl << "Congratulations" << endl << "Score: " << score << endl;
			setPlaneRandomLocation(); // Yeni bölüme uçak yeniden random konumlandırılarak başlanır 

		}
		Sleep(100);
	}
}
//<===========================================================================>
//<===========================================================================>
//<===========================================================================>
// Skorları dosya okuma işlemleriyle alır.  
string takeScores() {
	string scoreOfAll;
	char character;

	try {
		if (readData.is_open()) {
			while (readData.get(character)) {
				scoreOfAll.push_back(character);
			}
			readData.close();
		}
	}
	catch (int errorCode) {
		cout << "Data Read Error: " << errorCode;
	}
	return scoreOfAll;
}
//<===========================================================================>
// Ham olarak alınan skor çeşitli işlemlere tabi tutularak istenen formata getirilir.
// İstenen formata getirildikten sonra arraylistlerde tutulur. 
// Bunu yapabilmek için bir algoritma geliştirilimiştir.
// void scoresSplit() {
/*
void scoresSplit() {
	
// ! Kaldırıldı... Bazı zamanlar while içindeki şartlar için okuma erişim ihlal hatası veriyor
// Dolayısıyla scoreboard konsept gösterim için, statik gösterilecek
// İleri versiyonlarda düzeltilmeli

	string scoreOfAll = takeScores();
	char character[3000];

	int i = 0;
	while (i != scoreOfAll.length()) {
		character[i] = scoreOfAll.at(i);
		i++;
	}
	character[i] = '\0';

	//<=================================>

	int j = 0;
	while (character[j] != '\0') {

		string tempName;
		string tempScore;

		while (character[j] != '>') {
			tempName.push_back(character[j]);
			j++;
		}
		j++;


		while (character[j] != '|') {
			tempScore.push_back(character[j]);
			j++;
		}
		j++;

		arrayOfName.push_back(tempName);
		arrayOfScore.push_back(tempScore);
	}
}
*/
//<===========================================================================>
//<==========================Helicopters Loop=================================>
//<===========================================================================>
void timerLoop(int v) {

	//Helikopter 1 
	//-------------------------------------------------------------------------

	// Sağa Hareket
	if (heli1_DescriptionStatus == 0)
		heli1_x += heli1_movement_pixel_size;

	// Sola Hareket
	if (heli1_DescriptionStatus == 1)
		heli1_x -= heli1_movement_pixel_size;


	// Her turda yapılacak işlemler
	if (heli1_x < most_left - 50 || heli1_x > most_rigth + 50) {

		// Her turda random renk
		randomColor(1);

		// Her turda random hız tayini
		heli1_movement_pixel_size = randomSpeed(difficulty);

		// Her turda random yön tayini
		switch (randomDescription()) {
			// Helikopter sol tarafa atandı, sağa gidecek.
		case 0:
			heli1_x = most_left - 50;
			heli1_DescriptionStatus = 0;
			break;
			// Helikopter sağ tarafa atandı, sola gidecek.
		case 1:
			heli1_x = most_rigth + 50;
			heli1_DescriptionStatus = 1;
			break;
		}
	}
	//Helikopter 2
	//-------------------------------------------------------------------------

	// Sağa Hareket
	if (heli2_DescriptionStatus == 0)
		heli2_x += heli2_movement_pixel_size;

	// Sola Hareket
	if (heli2_DescriptionStatus == 1)
		heli2_x -= heli2_movement_pixel_size;


	// Her turda yapılacak işlemler
	if (heli2_x < most_left - 50 || heli2_x > most_rigth + 50) {

		// Her turda random renk
		randomColor(2);

		// Her turda random hız tayini
		heli2_movement_pixel_size = randomSpeed(difficulty);

		// Her turda random yön tayini
		switch (randomDescription()) {
			// Helikopter sol tarafa atandı, sağa gidecek.
		case 0:
			heli2_x = most_left - 50;
			heli2_DescriptionStatus = 0;
			break;
			// Helikopter sağ tarafa atandı, sola gidecek.
		case 1:
			heli2_x = most_rigth + 50;
			heli2_DescriptionStatus = 1;
			break;
		}
	}
	//Helikopter 3
	//-------------------------------------------------------------------------

	// Sağa Hareket
	if (heli3_DescriptionStatus == 0)
		heli3_x += heli3_movement_pixel_size;

	// Sola Hareket
	if (heli3_DescriptionStatus == 1)
		heli3_x -= heli3_movement_pixel_size;


	// Her turda yapılacak işlemler
	if (heli3_x < most_left - 50 || heli3_x > most_rigth + 50) {

		// Her turda random renk
		randomColor(3);

		// Her turda random hız tayini
		heli3_movement_pixel_size = randomSpeed(difficulty);

		// Her turda random yön tayini
		switch (randomDescription()) {
			// Helikopter sol tarafa atandı, sağa gidecek.
		case 0:
			heli3_x = most_left - 50;
			heli3_DescriptionStatus = 0;
			break;
			// Helikopter sağ tarafa atandı, sola gidecek.
		case 1:
			heli3_x = most_rigth + 50;
			heli3_DescriptionStatus = 1;
			break;
		}
	}
	//Helikopter 4
	//-------------------------------------------------------------------------

	// Sağa Hareket
	if (heli4_DescriptionStatus == 0)
		heli4_x += heli4_movement_pixel_size;

	// Sola Hareket
	if (heli4_DescriptionStatus == 1)
		heli4_x -= heli4_movement_pixel_size;


	// Her turda yapılacak işlemler
	if (heli4_x < most_left - 50 || heli4_x > most_rigth + 50) {

		// Her turda random renk
		randomColor(4);

		// Her turda random hız tayini
		heli4_movement_pixel_size = randomSpeed(difficulty);

		// Her turda random yön tayini
		switch (randomDescription()) {
			// Helikopter sol tarafa atandı, sağa gidecek.
		case 0:
			heli4_x = most_left - 50;
			heli4_DescriptionStatus = 0;
			break;
			// Helikopter sağ tarafa atandı, sola gidecek.
		case 1:
			heli4_x = most_rigth + 50;
			heli4_DescriptionStatus = 1;
			break;
		}
	}

	//-------------------------------------------------------------------------
	// Giriş Ekranı Animasyonu
	if(animation_quad_x < 265 && animation_quad_y == 450){
		animation_quad_x +=animation_quad_movement_pixel_size;
		animation_quad2_x -=animation_quad_movement_pixel_size; 		
	}
	if(animation_quad_x == 265 && animation_quad_y <= 500){
		animation_quad_y +=animation_quad_movement_pixel_size;
		animation_quad2_y -=animation_quad_movement_pixel_size; 			
	}
	if(animation_quad_x > 215 && animation_quad_y == 500){
		animation_quad_x -=animation_quad_movement_pixel_size; 		
		animation_quad2_x +=animation_quad_movement_pixel_size;
	}
	if(animation_quad_x == 215 && animation_quad_y > 450){
		animation_quad_y -=animation_quad_movement_pixel_size; 	
		animation_quad2_y +=animation_quad_movement_pixel_size;	
	}
	//-------------------------------------------------------------------------
	
	glutPostRedisplay(); // Görüntünün Sürekli Güncellenmesi Sağlanır Böylece Akıcı Bir Görüntü Elde Edilir
	glutTimerFunc(1000 / FPS, timerLoop, v);
}
//<===========================================================================>
void reshape(GLint x, GLint y) {
	glMatrixMode(GL_PROJECTION); // Projeksiyon modunda görünüm sağlanır
	glLoadIdentity(); // Matris dönşüm işlemi için gerekli
	glViewport(0, 0, x, y); // Uç Sınırlar Belirlenir | Sol 0'dan Sağ x'e(480) | Alt 0'dan Üst y'ye(640)
	gluOrtho2D(0.0, x, 0.0, y); // sol,sağ,üst,alt | 2 Boyutlu GÖrüntüleme Bölgesi Oluşturur
}
//<===========================================================================>
int main(int argc, char* argv[]) {

	//scoresSplit(); // Skor verilerini alıp parse edip name ve skor arraylistlerinde tutar. Sonra skor tablosuna aktarır.
	cout << takeScores() << endl; // Terminale dosyadan okunan veriler yazdırılır.

	setPlaneRandomLocation(); // Uçak Rastgele Konumlandırma
	thread threadImpactControll(impactControll); //Thread Yapısı İle Çarpışma Ve Oyunu Bitirme Kontrolü

	glutInit(&argc, argv); // Standart, GLUT Initialize
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Görüntüleme Modu RGB ve Çift Buffer ayarlandı

	glutInitWindowSize(window_width, window_height); // Pencere Boyutu | x -> 480, y -> 640
	glutInitWindowPosition(0, 0); // Pencerenin Ekrandaki Konumu 
	glutCreateWindow("160322045-RamazanEren-Sayar"); // Pencere Başlığı
	glClearColor(1, 1, 1, 0); // Arka Plan Rengi | Beyaz
	glutReshapeFunc(reshape); // Gerekli görünüm ayarlarının yapıldığı fonksiyon
	glutDisplayFunc(displayHome); // Nesneler çağrıldı
	glutKeyboardFunc(keyboardFuncHome); // KeyboardFunc callback fonksiyonun içerdiği klavye tuşları dinlenmeye alındı.
	glutMouseFunc(mouseHome); // Mouse callback fonksiyonu
	glutTimerFunc(1000 / FPS, timerLoop, 0); // Glut ekosistemi içinde bulunan döngü fonksiyonu 

	glutMainLoop();	// Standart GLUT döngüsü

	return 0;
}