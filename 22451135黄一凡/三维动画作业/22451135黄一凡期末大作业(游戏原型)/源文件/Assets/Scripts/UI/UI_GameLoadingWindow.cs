using UnityEngine;
using UnityEngine.UI;
namespace JKFrame
{
    [UIElement(true, "UI/UI_GameLoadingWindow", 4)]
    public class UI_GameLoadingWindow : UI_WindowBase
    {
        [SerializeField]
        private Text progress_Text;
        [SerializeField]
        private Image fill_Image;
        public override void OnShow()
        {
            base.OnShow();
            UpdateProgress(0);
        }

        /// <summary>
        /// 更新进度
        /// </summary>
        public void UpdateProgress(float progressValue)
        {
            progress_Text.text = (int)(progressValue) + "%";
            fill_Image.fillAmount = (int)(progressValue);
        }
    }
}