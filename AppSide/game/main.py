# ビー玉迷路ゲーム
from direct.showbase.ShowBase import ShowBase
import sys
import serial

# テキスト表示
from direct.gui.OnscreenText import OnscreenText
from panda3d.core import TextNode

# 衝突検出マスク
from panda3d.core import BitMask32

# 衝突判定
from panda3d.core import CollisionTraverser, CollisionNode
from panda3d.core import CollisionHandlerQueue, CollisionRay

# ボールの速度計算
from panda3d.core import LVector3, LRotationf
ACCEL = 70
MAX_SPEED = 5
MAX_SPEED_SQ = MAX_SPEED ** 2

# インターバル
from direct.interval.MetaInterval import Sequence, Parallel
from direct.interval.LerpInterval import LerpFunc
from direct.interval.FunctionInterval import Func, Wait

# ゴールの衝突判定
from panda3d.core import CollisionBox, Point3

# ライトと素材
from panda3d.core import AmbientLight, DirectionalLight
from panda3d.core import Material

#シリアル通信
SERIAL_COM = "COM3"
SERIAL_BPS = 115200

SERIAL_DATA_SIZE = 5

# グローバル変数の定義
sdata_list = []
angle_x = 0
angle_y = 0


class BallInMazeDemo(ShowBase):
	# init関数の定義
	def __init__(self):

		# 画面の生成
		ShowBase.__init__(self)

		# カメラの設定
		self.disableMouse()	# カメラマウス制御 OFF
		camera.setPosHpr(0, -9, 23, 0, -70, 0)	# カメラの位置

		# Escキーでプログラムの終了
		self.accept("escape", sys.exit)

		# テキストの表示
		# windowsの場合
		font = loader.loadFont('/c/Windows/Fonts/msgothic.ttc')
		# macの場合
		# font = loader.loadFont('/System/Library/Fonts/Hiragino Sans GB.ttc')

		# タイトル文字を表示する
		self.title = \
			OnscreenText(text="ボールをゴール地点まで運べ!",
						 parent=base.a2dBottomRight, align=TextNode.ARight,
						 fg=(1, 1, 1, 1), pos=(-0.1, 0.1), scale=.08, font=font,
						 shadow=(0, 0, 0, 0.5))

		# サブタイトル文字を表示する
		self.instructions = \
			OnscreenText(text="ジャイロセンサを動かすと迷路を傾けることができます",
						 parent=base.a2dTopLeft, align=TextNode.ALeft,
						 fg=(1, 1, 1, 1), pos=(0.1, -0.15), scale=.06, font=font,
						 shadow=(0, 0, 0, 0.5))

		# 迷路オブジェクトの設定
		self.maze = loader.loadModel("models/maze")
		self.maze.reparentTo(render)
		# 迷路の壁の衝突検出用マスク
		self.walls = self.maze.find("**/wall_collide")
		self.walls.node().setIntoCollideMask(BitMask32.bit(0))
		# 迷路の地面の衝突検出用マスク
		self.mazeGround = self.maze.find("**/ground_collide")
		self.mazeGround.node().setIntoCollideMask(BitMask32.bit(1))
		# 迷路の穴の衝突検出用マスク
		self.loseTriggers = []
		for i in range(6):
			trigger = self.maze.find("**/hole_collide" + str(i))
			trigger.node().setIntoCollideMask(BitMask32.bit(0))
			trigger.node().setName("loseTrigger")
			self.loseTriggers.append(trigger)

		# ボールオブジェクトの設定
		self.ballRoot = render.attachNewNode("ballRoot")
		self.ball = loader.loadModel("models/ball")
		self.ball.reparentTo(self.ballRoot)
		# ボール衝突検出用のマスク
		self.ballSphere = self.ball.find("**/ball")
		self.ballSphere.node().setFromCollideMask(BitMask32.bit(0))
		self.ballSphere.node().setIntoCollideMask(BitMask32.allOff())

		#ボールが床と衝突したときの位置を知る光線
		self.ballGroundRay = CollisionRay()
		self.ballGroundRay.setOrigin(0, 0, 10)
		self.ballGroundRay.setDirection(0, 0, -1)
		#光線の衝突検出用のマスク
		self.ballGroundCol = CollisionNode('groundRay')
		self.ballGroundCol.addSolid(self.ballGroundRay)
		self.ballGroundCol.setFromCollideMask(BitMask32.bit(1))
		self.ballGroundCol.setIntoCollideMask(BitMask32.allOff())
		self.ballGroundColNp = self.ballRoot.attachNewNode(self.ballGroundCol)

		# 衝突ハンドラーにボールと光線を追加
		self.cTrav = CollisionTraverser()
		self.cHandler = CollisionHandlerQueue()
		self.cTrav.addCollider(self.ballSphere, self.cHandler)
		self.cTrav.addCollider(self.ballGroundColNp, self.cHandler)

		# ゴールオブジェクトの設定
		self.goal = self.loader.loadModel("models/misc/rgbCube")
		self.goal.reparentTo(self.render)
		self.goal.setScale(0.1, 0.1, 0.1)
		self.goal.setPos(4, 4, -0.5)
		c = CollisionNode('goalCol')
		c.addSolid(CollisionBox(Point3(0, 0, 0), 3, 3, 3))
		c.setIntoCollideMask(BitMask32.bit(0))
		goal_c = self.goal.attachNewNode(c)

		# ライトの設定
		ambientLight = AmbientLight("ambientLight")
		ambientLight.setColor((.55, .55, .55, 1))
		directionalLight = DirectionalLight("directionalLight")
		directionalLight.setDirection(LVector3(0, 0, -1))
		directionalLight.setColor((0.375, 0.375, 0.375, 1))
		directionalLight.setSpecularColor((1, 1, 1, 1))
		self.ballRoot.setLight(render.attachNewNode(ambientLight))
		self.ballRoot.setLight(render.attachNewNode(directionalLight))
		# 光沢のある素材の設定
		m = Material()
		m.setSpecular((1, 1, 1, 1))
		m.setShininess(96)
		self.ball.setMaterial(m, 1)

		# シリアル通信の初期化
		self.ser = serial.Serial(SERIAL_COM, SERIAL_BPS, timeout=0.000001)
		angle_x = 0
		angle_y = 0
		sdata_list.clear()

		# start関数の呼び出し
		self.start()

	# Start関数の定義
	def start(self):

		# ボールの初期位置の設定
		startPos = self.maze.find("**/start").getPos()
		self.ballRoot.setPos(startPos)
		self.ballV = LVector3(0, 0, 0)
		self.accelV = LVector3(0, 0, 0)
		
		# シリアル通信の入力バッファをクリアする
		self.ser.reset_input_buffer()

		# rollTask関数の呼び出し
		taskMgr.remove("rollTask")
		self.mainLoop = taskMgr.add(self.rollTask, "rollTask")

	# rollTask関数の定義
	def rollTask(self, task):
		dt = globalClock.getDt()
		if dt > .2:
			return task.cont

		# ボールが衝突したときの処理の分岐
		for i in range(self.cHandler.getNumEntries()):
			entry = self.cHandler.getEntry(i)
			name = entry.getIntoNode().getName()
			if name == "wall_collide":
				self.wallCollideHandler(entry)
			elif name == "ground_collide":
				self.groundCollideHandler(entry)
			elif name == "loseTrigger":
				self.loseGame(entry)
			elif name == "goalCol":
				self.winGame(entry)

		# ボールの速度や向きの計算
		self.ballV += self.accelV * dt * ACCEL
		if self.ballV.lengthSquared() > MAX_SPEED_SQ:
			self.ballV.normalize()
			self.ballV *= MAX_SPEED
		self.ballRoot.setPos(self.ballRoot.getPos() + (self.ballV * dt))
		prevRot = LRotationf(self.ball.getQuat())
		axis = LVector3.up().cross(self.ballV)
		newRot = LRotationf(axis, 45.5 * dt * self.ballV.length())
		self.ball.setQuat(prevRot * newRot)

		# シリアル通信データの取得
		self.getSerialData()
		self.getXYData()
		# print(angle_x, ',', angle_y)
		self.maze.setP(angle_y)
		self.maze.setR(angle_x)

		# 迷路の傾きのマウス操作
		#if base.mouseWatcherNode.hasMouse():
			#mpos = base.mouseWatcherNode.getMouse()
			#self.maze.setP(mpos.getY() * -10)
			#self.maze.setR(mpos.getX() * 10)
			

		return task.cont

	# groundCollideHandler関数の定義
	def groundCollideHandler(self, colEntry):
		newZ = colEntry.getSurfacePoint(render).getZ()
		self.ballRoot.setZ(newZ + .4)

		norm = colEntry.getSurfaceNormal(render)
		accelSide = norm.cross(LVector3.up())
		self.accelV = norm.cross(accelSide)

	# wallCollideHandler関数の定義
	def wallCollideHandler(self, colEntry):
		norm = colEntry.getSurfaceNormal(render) * -1	# 壁の法線
		curSpeed = self.ballV.length()	# 現在の速度
		inVec = self.ballV / curSpeed	# 進行方向
		velAngle = norm.dot(inVec)	# 角度
		hitDir = colEntry.getSurfacePoint(render) - self.ballRoot.getPos()
		hitDir.normalize()

		hitAngle = norm.dot(hitDir)

		if velAngle > 0 and hitAngle > .995:
			reflectVec = (norm * norm.dot(inVec * -1) * 2) +inVec
			self.ballV = reflectVec * (curSpeed * (((1 - velAngle) * .5) + .5))
			disp = (colEntry.getSurfacePoint(render) -
					colEntry.getInteriorPoint(render))
			newPos = self.ballRoot.getPos() + disp
			self.ballRoot.setPos(newPos)

	# loseGame関数の定義
	def loseGame(self, entry):
		toPos = entry.getInteriorPoint(render)
		taskMgr.remove('rollTask')

		#ボールを穴に落とした後最初の位置に移動させてから再度スタート
		Sequence(
			Parallel(
				LerpFunc(self.ballRoot.setX, fromData=self.ballRoot.getX(),
						 toData=toPos.getX(), duration=.1),
				LerpFunc(self.ballRoot.setY, fromData=self.ballRoot.getY(),
						 toData=toPos.getY(), duration=.1),
				LerpFunc(self.ballRoot.setZ, fromData=self.ballRoot.getZ(),
						 toData=self.ballRoot.getZ() - .9, duration=.2)),
			Wait(1),
			Func(self.start)).start()

	# winGame関数の定義
	def winGame(self, entry):
		self.title.setText("ゴール!!")
		toPos = entry.getInteriorPoint(render)
		self.ballRoot.hide()
		taskMgr.remove('rollTask')
	
	# シリアル通信データの取得
	def getSerialData(self):
		while True:
			#self.ser.reset_input_buffer()
			sbuff = self.ser.read()	# 1Byteずつ読み出し
			if sbuff == b'':
				break

			sdata_list.append(int.from_bytes(sbuff, 'big'))	# 配列の末尾に追加
			if len(sdata_list) > 9:	# 配列の要素数が9より大きい場合
				sdata_list.pop(0)	# 配列の先頭を削除

	# 受信したデータからX軸・Y軸情報を抽出
	def getXYData(self):
		global angle_x
		global angle_y
		if len(sdata_list) == 9:
			for i in reversed(range(5)):		#4, 3, 2, 1, 0の順
				result = self.checkChkSum(*sdata_list[i:(i + 5)])
				if result == True:
					angle_x = (((sdata_list[i] << 8) + sdata_list[i + 1]) - 1800) / 10
					angle_y = (((sdata_list[i + 2] << 8) + sdata_list[i + 3]) - 1800) /10
					break

	# チェックサムチェック
	def checkChkSum(self, *args):
		if args[4] == (sum(args[0:4]) & 255):	#データ部(4Byte)の合計とChkSumが一致するか？
			ret = True
		else:
			ret = False
		return ret


game = BallInMazeDemo()
game.run()
