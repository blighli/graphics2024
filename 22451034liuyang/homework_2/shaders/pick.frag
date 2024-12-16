#version 330

out vec4 FragColor;

uniform int code;

void main()
{

    FragColor = vec4(code/255.0F,0.0f,0.0f,0.0f); // 透明不显示
}
