<template>
  <div>
    <button @click="startGame">开始游戏</button>
    <button @click="showSettings = true">设置</button>
    <button @click="resetGame">重置</button> <!-- 新增重置按钮 -->
    <div v-if="showSettings" class="settings-panel">
      <label>
        小球大小:
        <input type="number" v-model="ballSize" min="10" max="100" />
      </label>
      <label>
        小球持续时间 (秒):
        <input type="number" v-model="ballDuration" min="1" max="10" />
      </label>
      <button @click="showSettings = false">关闭设置</button>
    </div>
    <div class="score-board">得分: {{ score }}</div>
    <div class="countdown-board">时间: {{ countdown }} 秒</div> <!-- 新增倒计时显示 -->
    <canvas ref="gameCanvas" width="800" height="600"></canvas>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue';

const gameCanvas = ref<HTMLCanvasElement | null>(null);
const score = ref(0);
const ballSize = ref(20);
const ballDuration = ref(3);
const showSettings = ref(false);
let gameInterval: NodeJS.Timeout | null = null;
let ball: { x: number; y: number; startTime: number; scale: number; shrinking: boolean } | null = null;
let countdown = ref(60); // 新增倒计时变量
let bullets: { x: number; y: number; vy: number }[] = []; // 新增子弹数组

let gl: WebGLRenderingContext | null = null;
let ballProgram: WebGLProgram | null = null;
let bulletProgram: WebGLProgram | null = null;
let ballBuffer: WebGLBuffer | null = null;
let bulletBuffer: WebGLBuffer | null = null;

const initWebGL = () => {
  if (gameCanvas.value) {
    gl = gameCanvas.value.getContext('webgl');
    if (!gl) {
      console.error('WebGL not supported');
      return;
    }
    gl.clearColor(0.0, 0.0, 0.0, 1.0); // 设置背景颜色为黑色
    gl.clear(gl.COLOR_BUFFER_BIT);

    // 创建和编译着色器程序
    ballProgram = createProgram(gl, vertexShaderSource, ballFragmentShaderSource);
    bulletProgram = createProgram(gl, vertexShaderSource, bulletFragmentShaderSource);

    // 创建和绑定缓冲区
    ballBuffer = gl.createBuffer();
    bulletBuffer = gl.createBuffer();

    // 加载纹理图片
    const ballTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, ballTexture);
    const level = 0;
    const internalFormat = gl.RGBA;
    const width = 1;
    const height = 1;
    const border = 0;
    const srcFormat = gl.RGBA;
    const srcType = gl.UNSIGNED_BYTE;
    const pixel = new Uint8Array([0, 0, 255, 255]);  // opaque blue
    gl.texImage2D(gl.TEXTURE_2D, level, internalFormat, width, height, border, srcFormat, srcType, pixel);

    const image = new Image();
    image.onload = () => {
      gl.bindTexture(gl.TEXTURE_2D, ballTexture);
      gl.texImage2D(gl.TEXTURE_2D, level, internalFormat, srcFormat, srcType, image);

      if (isPowerOf2(image.width) && isPowerOf2(image.height)) {
        gl.generateMipmap(gl.TEXTURE_2D);
      } else {
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
      }
    };
    image.src = 'sun_texture.png'; // 替换为实际路径
  }
};

const createShader = (gl: WebGLRenderingContext, type: number, source: string): WebGLShader | null => {
  const shader = gl.createShader(type);
  if (!shader) return null;
  gl.shaderSource(shader, source);
  gl.compileShader(shader);
  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    console.error('Error compiling shader:', gl.getShaderInfoLog(shader));
    gl.deleteShader(shader);
    return null;
  }
  return shader;
};

const createProgram = (gl: WebGLRenderingContext, vertexShaderSource: string, fragmentShaderSource: string): WebGLProgram | null => {
  const vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
  const fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);
  if (!vertexShader || !fragmentShader) return null;

  const program = gl.createProgram();
  if (!program) return null;
  gl.attachShader(program, vertexShader);
  gl.attachShader(program, fragmentShader);
  gl.linkProgram(program);
  if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
    console.error('Error linking program:', gl.getProgramInfoLog(program));
    gl.deleteProgram(program);
    return null;
  }
  return program;
};

const vertexShaderSource = `
attribute vec2 position;
attribute vec2 texCoord;
varying vec2 v_texCoord;
void main() {
  gl_Position = vec4(position, 0.0, 1.0);
  v_texCoord = texCoord;
}
`;

const ballFragmentShaderSource = `
precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D u_texture;
void main() {
  gl_FragColor = texture2D(u_texture, v_texCoord);
}
`;

const bulletFragmentShaderSource = `
void main() {
  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); // 白色
}
`;

const startGame = () => {
  score.value = 0;
  countdown.value = 60; // 重置倒计时
  if (gameInterval) clearInterval(gameInterval);
  gameInterval = setInterval(() => {
    if (ball) {
      const currentTime = Date.now();
      if (currentTime - ball.startTime >= ballDuration.value * 1000) {
        ball = null;
        spawnBall(); // 重新生成一个新的小球
      }
    } else {
      spawnBall(); // 生成新的小球
    }
    updateBullets(); // 更新子弹位置
  }, 100); // 每100毫秒检查一次小球是否需要刷新
  spawnBall();
  startCountdown(); // 启动倒计时
};

const spawnBall = () => {
  if (gl && ballBuffer && gameCanvas.value) {
    ball = {
      x: Math.random() * (gameCanvas.value.width - ballSize.value), // 确保小球完全显示在画布内
      y: Math.random() * (gameCanvas.value.height - ballSize.value), // 确保小球完全显示在画布内
      startTime: Date.now(),
      scale: 1.0, // 初始化缩放比例
      shrinking: false, // 标记小球是否正在缩小
    };
    const halfSize = ballSize.value / 2;
    const vertices = [
      (ball.x - halfSize) / 400 - 1, (ball.y + halfSize) / 300 - 1, 0.0, 1.0,
      (ball.x + halfSize) / 400 - 1, (ball.y + halfSize) / 300 - 1, 1.0, 1.0,
      (ball.x - halfSize) / 400 - 1, (ball.y - halfSize) / 300 - 1, 0.0, 0.0,
      (ball.x + halfSize) / 400 - 1, (ball.y + halfSize) / 300 - 1, 1.0, 1.0,
      (ball.x + halfSize) / 400 - 1, (ball.y - halfSize) / 300 - 1, 1.0, 0.0,
      (ball.x - halfSize) / 400 - 1, (ball.y - halfSize) / 300 - 1, 0.0, 0.0,
    ];
    gl.bindBuffer(gl.ARRAY_BUFFER, ballBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
  }
};

const drawBall = () => {
  if (gl && ballProgram && ballBuffer && ball) {
    gl.useProgram(ballProgram);
    const positionLocation = gl.getAttribLocation(ballProgram, 'position');
    gl.enableVertexAttribArray(positionLocation);
    gl.bindBuffer(gl.ARRAY_BUFFER, ballBuffer);

    // 更新顶点数据以反映缩放
    const halfSize = ballSize.value / 2 * ball.scale;
    const vertices = [
      (ball.x - halfSize) / 400 - 1, (ball.y + halfSize) / 300 - 1, 0.0, 1.0,
      (ball.x + halfSize) / 400 - 1, (ball.y + halfSize) / 300 - 1, 1.0, 1.0,
      (ball.x - halfSize) / 400 - 1, (ball.y - halfSize) / 300 - 1, 0.0, 0.0,
      (ball.x + halfSize) / 400 - 1, (ball.y + halfSize) / 300 - 1, 1.0, 1.0,
      (ball.x + halfSize) / 400 - 1, (ball.y - halfSize) / 300 - 1, 1.0, 0.0,
      (ball.x - halfSize) / 400 - 1, (ball.y - halfSize) / 300 - 1, 0.0, 0.0,
    ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

    gl.vertexAttribPointer(positionLocation, 2, gl.FLOAT, false, 16, 0);

    const texCoordLocation = gl.getAttribLocation(ballProgram, 'texCoord');
    gl.enableVertexAttribArray(texCoordLocation);
    gl.vertexAttribPointer(texCoordLocation, 2, gl.FLOAT, false, 16, 8);

    const textureLocation = gl.getUniformLocation(ballProgram, 'u_texture');
    gl.uniform1i(textureLocation, 0);

    gl.drawArrays(gl.TRIANGLES, 0, 6);

    // 处理缩放动画
    if (ball.shrinking) {
      ball.scale -= 0.05; // 每帧缩小0.05
      if (ball.scale <= 0) {
        ball.shrinking = false;
        ball = null; // 小球完全缩小后消失
        spawnBall(); // 生成新的小球
      }
    }
  }
};

const checkClick = (event: MouseEvent) => {
  if (ball && gameCanvas.value) {
    const rect = gameCanvas.value.getBoundingClientRect();
    const mouseX = event.clientX - rect.left;
    const mouseY = event.clientY - rect.top;
    // 修正WebGL坐标系转换
    const glX = (mouseX / rect.width) * 2 - 1;
    const glY = -(mouseY / rect.height) * 2 + 1;
    if (
      glX > (ball.x - ballSize.value / 2) / 400 - 1 &&
      glX < (ball.x + ballSize.value / 2) / 400 - 1 &&
      glY > (ball.y - ballSize.value / 2) / 300 - 1 &&
      glY < (ball.y + ballSize.value / 2) / 300 - 1
    ) {
      score.value++;
      ball.scale = 1.0; // 初始化缩放比例
      ball.shrinking = true; // 标记小球正在缩小
      // 添加爆炸效果
      createExplosion(glX, glY);
    }
  }
  if (gameCanvas.value) {
    const rect = gameCanvas.value.getBoundingClientRect();
    const mouseX = event.clientX - rect.left;
    const mouseY = event.clientY - rect.top;
    // 修正WebGL坐标系转换
    const glX = (mouseX / rect.width) * 2 - 1;
    const glY = -(mouseY / rect.height) * 2 + 1;
    bullets.push({ x: glX, y: glY, vy: -0.05 }); // 添加子弹，调整vy以适应新的坐标系
  }
};

const resetGame = () => { // 新增重置游戏功能
  score.value = 0;
  countdown.value = 60; // 重置倒计时
  if (gameInterval) clearInterval(gameInterval);
  gameInterval = null;
  ball = null;
  bullets = [];
  if (gl) {
    gl.clear(gl.COLOR_BUFFER_BIT);
  }
};

const updateBullets = () => {
  if (gl && bulletBuffer) {
    bullets = bullets.filter(bullet => bullet.y > 0); // 移除超出画布的子弹
    bullets.forEach(bullet => {
      bullet.y += bullet.vy; // 更新子弹位置
    });
    const vertices = bullets.flatMap(bullet => [
      (bullet.x - 2) / 400 - 1, (bullet.y + 2) / 300 - 1,
      (bullet.x + 2) / 400 - 1, (bullet.y + 2) / 300 - 1,
      (bullet.x - 2) / 400 - 1, (bullet.y - 2) / 300 - 1,
      (bullet.x + 2) / 400 - 1, (bullet.y + 2) / 300 - 1,
      (bullet.x + 2) / 400 - 1, (bullet.y - 2) / 300 - 1,
      (bullet.x - 2) / 400 - 1, (bullet.y - 2) / 300 - 1,
    ]);
    gl.bindBuffer(gl.ARRAY_BUFFER, bulletBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
    drawBullets();
  }
};

const drawBullets = () => {
  if (gl && bulletProgram && bulletBuffer) {
    gl.useProgram(bulletProgram);
    const positionLocation = gl.getAttribLocation(bulletProgram, 'position');
    gl.enableVertexAttribArray(positionLocation);
    gl.bindBuffer(gl.ARRAY_BUFFER, bulletBuffer);
    gl.vertexAttribPointer(positionLocation, 2, gl.FLOAT, false, 0, 0);
    gl.drawArrays(gl.TRIANGLES, 0, bullets.length * 6);
  }
};

const drawFrame = () => {
  if (gl) {
    gl.clear(gl.COLOR_BUFFER_BIT);
    drawBall();
    drawBullets();
  }
  requestAnimationFrame(drawFrame);
};

const startCountdown = () => {
  let countdownInterval = setInterval(() => {
    if (countdown.value > 0) {
      countdown.value--;
    } else {
      clearInterval(countdownInterval);
      clearInterval(gameInterval!);
      gameInterval = null;
      ball = null;
      bullets = [];
      if (gl) {
        gl.clear(gl.COLOR_BUFFER_BIT);
      }
    }
  }, 1000);
};

const createExplosion = (x: number, y: number) => {
  let explosionSize = 1.0;
  const explosionInterval = setInterval(() => {
    if (explosionSize <= 0) {
      clearInterval(explosionInterval);
    } else {
      drawExplosion(x, y, explosionSize);
      explosionSize -= 0.1;
    }
  }, 100);
};

const drawExplosion = (x: number, y: number, size: number) => {
  if (gl) {
    gl.useProgram(bulletProgram);
    const positionLocation = gl.getAttribLocation(bulletProgram, 'position');
    gl.enableVertexAttribArray(positionLocation);

    const vertices = [
      x - size / 2, y - size / 2,
      x + size / 2, y - size / 2,
      x - size / 2, y + size / 2,
      x + size / 2, y - size / 2,
      x + size / 2, y + size / 2,
      x - size / 2, y + size / 2,
    ];

    gl.bindBuffer(gl.ARRAY_BUFFER, bulletBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
    gl.vertexAttribPointer(positionLocation, 2, gl.FLOAT, false, 0, 0);
    gl.drawArrays(gl.TRIANGLES, 0, 6);
  }
};

const isPowerOf2 = (value: number): boolean => {
  return (value & (value - 1)) === 0;
};

onMounted(() => {
  initWebGL();
  window.addEventListener('click', checkClick);
  drawFrame(); // 启动绘制循环
});

onUnmounted(() => {
  window.removeEventListener('click', checkClick);
  if (gameInterval) clearInterval(gameInterval);
});
</script>

<style scoped>
.settings-panel {
  position: absolute;
  top: 10px;
  right: 10px;
  background: white;
  padding: 10px;
  border: 1px solid black;
}

.score-board {
  position: absolute;
  top: 10px;
  left: 10px;
  color: blue;
}

.countdown-board { /* 新增倒计时显示样式 */
  position: absolute;
  top: 40px;
  left: 10px;
  color: blue;
}

canvas {
  background-color: black; /* 设置游戏背景为黑色 */
}
</style>