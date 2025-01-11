import cv2
import numpy as np
import random
import time

# Constants
WIDTH, HEIGHT = 1000, 600  # Extended width of the image
BASKET_WIDTH, BASKET_HEIGHT = 100, 30
HEN_WIDTH, HEN_HEIGHT = 80, 60
EGG_WIDTH, EGG_HEIGHT = 20, 30
ROTTEN_EGG_WIDTH, ROTTEN_EGG_HEIGHT = 20, 30
INITIAL_EGG_SPEED = 3
BASKET_SPEED = 20
HEN_SPEED = 1  # Reduced hen speed
TEAM_NAME = "22451052"
TEAM_MEMBERS = ["qymm",]
LOGO_PATH = "./textures/sun.jpg"  # Replace with the actual path to the logo image
ASSISTANT_PROFESSOR = "press S to start"

# Initialize positions
hens = [(WIDTH // 2, 50)]
basket_pos = (WIDTH // 2, HEIGHT - 50)
eggs = []
rotten_eggs = []
score = 0
high_score = 0
egg_speed = INITIAL_EGG_SPEED
hen_direction = 1  # 1 for right, -1 for left
last_egg_drop_time = time.time()

def draw_hen(image, pos):
    # Draw body
    cv2.ellipse(image, (pos[0], pos[1]), (HEN_WIDTH // 2, HEN_HEIGHT // 2), 0, 0, 360, (0, 255, 0), -1)
    # Draw head
    cv2.circle(image, (pos[0], pos[1] - HEN_HEIGHT // 2), HEN_WIDTH // 4, (0, 200, 0), -1)
    # Draw beak
    pts = np.array([[pos[0], pos[1] - HEN_HEIGHT // 2],
                    [pos[0] + 10, pos[1] - HEN_HEIGHT // 2 - 10],
                    [pos[0], pos[1] - HEN_HEIGHT // 2 + 10]], np.int32)
    cv2.polylines(image, [pts], isClosed=True, color=(0, 165, 255), thickness=1)

def draw_basket(image, pos):
    # Draw the basket
    cv2.rectangle(image, (pos[0] - BASKET_WIDTH // 2, pos[1] - BASKET_HEIGHT // 2),
                  (pos[0] + BASKET_WIDTH // 2, pos[1] + BASKET_HEIGHT // 2), (165, 42, 42), -1)
    # Draw the handle
    cv2.ellipse(image, (pos[0], pos[1] - BASKET_HEIGHT // 2), (BASKET_WIDTH // 2, BASKET_HEIGHT // 4), 0, 0, 180, (165, 42, 42), -1)

def draw_egg(image, pos):
    #cv2.ellipse(image, pos, (EGG_WIDTH // 2, EGG_HEIGHT // 2), 0, 0, 360, (255, 255, 255), -1)
    # 绘制3D效果蛋壳
    for i in range(EGG_HEIGHT // 2, 0, -3):
        color_intensity = 255 - int((i / (EGG_HEIGHT // 2)) * 100)  # 逐渐变深
        cv2.ellipse(image, pos, (EGG_WIDTH // 2 - i // 10, i), 0, 0, 360, (color_intensity, color_intensity, 255), -1)

    # 添加光照反射效果
    cv2.circle(image, (pos[0] - 5, pos[1] - 10), 5, (255, 255, 255), -1)

    # 阴影效果
    shadow_offset = 10
    shadow_intensity = 50
    cv2.ellipse(image, (pos[0], pos[1] + shadow_offset), (EGG_WIDTH // 2 + 5, 10), 0, 0, 360, (shadow_intensity, shadow_intensity, shadow_intensity), -1)

def draw_rotten_egg(image, pos):
    #cv2.ellipse(image, pos, (ROTTEN_EGG_WIDTH // 2, ROTTEN_EGG_HEIGHT // 2), 0, 0, 360, (0, 0, 255), -1)
    # 绘制3D效果腐烂蛋
    for i in range(ROTTEN_EGG_HEIGHT // 2, 0, -3):
        color_intensity = 150 - int((i / (ROTTEN_EGG_HEIGHT // 2)) * 100)  # 深红色
        cv2.ellipse(image, pos, (ROTTEN_EGG_WIDTH // 2 - i // 10, i), 0, 0, 360, (color_intensity, 0, 0), -1)

    # 光照反射效果
    cv2.circle(image, (pos[0] - 5, pos[1] - 10), 5, (180, 0, 0), -1)

    # 阴影效果
    shadow_offset = 10
    shadow_intensity = 40
    cv2.ellipse(image, (pos[0], pos[1] + shadow_offset), (ROTTEN_EGG_WIDTH // 2 + 5, 10), 0, 0, 360, (shadow_intensity, 0, 0), -1)

def drop_egg():
    global last_egg_drop_time
    current_time = time.time()
    if current_time - last_egg_drop_time > random.uniform(1.0, 3.0):  # Random interval between 1.0 and 3.0 seconds
        last_egg_drop_time = current_time
        selected_hen = random.choice(hens)
        if random.random() < 0.1:  # Random chance to drop a rotten egg
            rotten_eggs.append((selected_hen[0], selected_hen[1] + HEN_HEIGHT // 2))
        else:
            eggs.append((selected_hen[0], selected_hen[1] + HEN_HEIGHT // 2))

def reset_game():
    global hens, basket_pos, eggs, rotten_eggs, score, egg_speed, hen_direction, last_egg_drop_time
    hens = [(WIDTH // 2, 50)]
    basket_pos = (WIDTH // 2, HEIGHT - 50)
    eggs = []
    rotten_eggs = []
    score = 0
    egg_speed = INITIAL_EGG_SPEED
    hen_direction = 1
    last_egg_drop_time = time.time()

def draw_home_page(image):
    # Load and draw the logo at the top spanning from left to right
    logo = cv2.imread(LOGO_PATH)
    logo_height, logo_width = logo.shape[:2]
    logo_resized = cv2.resize(logo, (WIDTH, 100))
    image[0:100, 0:WIDTH] = logo_resized

    # Draw the project name below the logo
    cv2.putText(image, "Egg Catcher", (WIDTH // 2 - 120, 180), cv2.FONT_HERSHEY_SIMPLEX, 1.5, (255, 255, 255), 2)

    # Draw the start button
    button_text = "Start Game"
    (w, h), _ = cv2.getTextSize(button_text, cv2.FONT_HERSHEY_SIMPLEX, 2, 2)
    button_x = WIDTH // 2 - w // 2
    button_y = HEIGHT // 2 - h // 2
    cv2.rectangle(image, (button_x - 10, button_y - 10), (button_x + w + 10, button_y + h + 10), (0, 255, 0), -1)
    cv2.putText(image, button_text, (button_x, button_y + h + 5), cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 0, 0), 2)

    # Draw team name at the bottom
    cv2.putText(image, TEAM_NAME, (8, HEIGHT - 100), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)

    # Draw team members' names
    y_offset = HEIGHT - 40
    for member in TEAM_MEMBERS:
        cv2.putText(image, member, (5, y_offset), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (200, 200, 200), 1)
        y_offset += 30

    # Draw the assistant professor's name at the bottom right
    cv2.putText(image, ASSISTANT_PROFESSOR, (WIDTH - 300, HEIGHT - 20), cv2.FONT_HERSHEY_SIMPLEX, 0.50, (255, 255, 255), 2)

def main():
    # Create a blank image for the home page
    home_image = np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8)

    # Draw the home page elements
    draw_home_page(home_image)

    # Show the home page
    cv2.imshow("Hen Lays Eggs - Home", home_image)

    # Wait for user input to start the game
    while True:
        key = cv2.waitKey(0)
        if key == ord('s'):  # Press 's' to start the game
            break
        elif key == 27:  # Press ESC to quit
            cv2.destroyAllWindows()
            exit()

    # Start the game
    run_game()

def run_game():
    global score, high_score, hens, basket_pos, eggs, rotten_eggs, egg_speed, hen_direction, last_egg_drop_time
    
    # Main loop
    while True:
        reset_game()
        game_over = False
        while not game_over:
            # Create a blank image
            image = np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8)

            # Draw hens, basket, eggs, and rotten eggs
            for hen in hens:
                draw_hen(image, hen)
            draw_basket(image, basket_pos)
            for egg in eggs:
                draw_egg(image, egg)
            for rotten_egg in rotten_eggs:
                draw_rotten_egg(image, rotten_egg)

            # Show the score
            cv2.putText(image, f'Score: {score}', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

            # Show the image
            cv2.imshow("Hen Lays Eggs", image)

            # Move eggs
            new_eggs = []
            for egg in eggs:
                new_egg = (egg[0], egg[1] + egg_speed)
                if new_egg[1] + EGG_HEIGHT // 2 < HEIGHT:
                    if basket_pos[0] - BASKET_WIDTH // 2 <= new_egg[0] <= basket_pos[0] + BASKET_WIDTH // 2 and basket_pos[1] - BASKET_HEIGHT // 2 <= new_egg[1] + EGG_HEIGHT // 2 <= basket_pos[1] + BASKET_HEIGHT // 2:
                        score += 1
                        if score % 10 == 0:
                            # Add a new hen
                            if len(hens) * HEN_WIDTH < WIDTH:
                                new_hen_pos = (len(hens) * WIDTH // (len(hens) + 1), 50)
                                hens.append(new_hen_pos)
                            # Increase egg speed
                            egg_speed += 1
                    else:
                        new_eggs.append(new_egg)
                else:
                    game_over = True

            eggs = new_eggs

            # Move rotten eggs
            new_rotten_eggs = []
            for rotten_egg in rotten_eggs:
                new_rotten_egg = (rotten_egg[0], rotten_egg[1] + egg_speed)
                if new_rotten_egg[1] + ROTTEN_EGG_HEIGHT // 2 < HEIGHT:
                    if basket_pos[0] - BASKET_WIDTH // 2 <= new_rotten_egg[0] <= basket_pos[0] + BASKET_WIDTH // 2 and basket_pos[1] - BASKET_HEIGHT // 2 <= new_rotten_egg[1] + ROTTEN_EGG_HEIGHT // 2 <= basket_pos[1] + BASKET_HEIGHT // 2:
                        game_over = True
                    else:
                        new_rotten_eggs.append(new_rotten_egg)
                else:
                    game_over = False

            rotten_eggs = new_rotten_eggs

            # Drop a new egg at random intervals
            drop_egg()

            # Move the first hen
            if len(hens) > 0:
                hen_pos_x = hens[0][0] + hen_direction * HEN_SPEED
                if hen_pos_x - HEN_WIDTH // 2 <= 0 or hen_pos_x + HEN_WIDTH // 2 >= WIDTH:
                    hen_direction = -hen_direction
                hens[0] = (hen_pos_x, hens[0][1])

            # Handle user input
            key = cv2.waitKey(30)
            if key == ord('a') and basket_pos[0] - BASKET_WIDTH // 2 > 0:   # Move basket left
                basket_pos = (basket_pos[0] - BASKET_SPEED, basket_pos[1])
            elif key == ord('d') and basket_pos[0] + BASKET_WIDTH // 2 < WIDTH:  # Move basket right
                basket_pos = (basket_pos[0] + BASKET_SPEED, basket_pos[1])
            elif key == 27:  # Esc key to exit
                game_over = True
                break

        # Update high score
        if score > high_score:
            high_score = score

        # Display final score and high score
        image = np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8)
        cv2.putText(image, f'Game Over! Final Score: {score}', (100, HEIGHT // 2 - 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
        cv2.putText(image, f'High Score: {high_score}', (100, HEIGHT // 2), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
        cv2.putText(image, 'Press R to Restart or ESC to Quit', (100, HEIGHT // 2 + 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
        cv2.imshow("Hen Lays Eggs", image)

        # Wait for user input to restart or quit
        while True:
            key = cv2.waitKey(0)
            if key == ord('r'):  # Restart the game
                break
            elif key == 27:  # ESC key to quit
                cv2.destroyAllWindows()
                exit()
 
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
