using LogAnalyzerApp.Command;
using LogAnalyzerApp.Model;
using LogAnalyzerBridge;
using OxyPlot;
using OxyPlot.Series;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
namespace LogAnalyzerApp.ViewModel
{
    public class MainViewModel : INotifyPropertyChanged
    {
        public string FilePath { get; set; }

        public int InfoCount { get; set; }
        public int WarnCount { get; set; }
        public int ErrorCount { get; set; }

        public ObservableCollection<ModuleError> ModuleErrors { get; set; }
            = new ObservableCollection<ModuleError>();

        public ICommand BrowseCommand { get; }
        public ICommand AnalyzeCommand { get; }

        public MainViewModel()
        {
            BrowseCommand = new RelayCommand(BrowseFile);
            AnalyzeCommand = new RelayCommand(AnalyzeFile);
        }
        private AnalyzerWrapper _analyzer = new AnalyzerWrapper();
        private void BrowseFile()
        {
            var dialog = new Microsoft.Win32.OpenFileDialog();
            if (dialog.ShowDialog() == true)
            {
                FilePath = dialog.FileName;
                OnPropertyChanged(nameof(FilePath));
                Reset();
                ResetPieChart();
                StatusMessage = "Ready To Analyse.";
            }
        }
        private bool _isAnalyzing;
        public bool IsAnalyzing
        {
            get => _isAnalyzing;
            set
            {
                _isAnalyzing = value;
                OnPropertyChanged(nameof(IsAnalyzing));
                OnPropertyChanged(nameof(CanAnalyze));
            }
        }

        public bool CanAnalyze => !IsAnalyzing;

        private string _statusMessage = "Ready To Analyse.";

        public string StatusMessage
        {
            get => _statusMessage;
            set
            {
                _statusMessage = value;
                OnPropertyChanged(nameof(StatusMessage));
            }
        }
        public PlotModel PieModel { get; set; }

        private void CreatePieChart()
        {
            var model = new PlotModel
            {
                Title = "Log Distribution",
                TextColor = OxyColors.Black
            };

            var pie = new PieSeries
            {
                StrokeThickness = 0,
                InsideLabelPosition = 0.7,
                AngleSpan = 360,
                StartAngle = 0,
                TextColor = OxyColors.White
            };

            pie.Slices.Add(new PieSlice("Info", InfoCount) { Fill = OxyColors.Teal });
            pie.Slices.Add(new PieSlice("Warn", WarnCount) { Fill = OxyColors.Gold });
            pie.Slices.Add(new PieSlice("Error", ErrorCount) { Fill = OxyColors.Red });

            model.Series.Add(pie);

            PieModel = model;
        }
        private async void AnalyzeFile()
        {
            Reset();
            if (string.IsNullOrEmpty(FilePath) || IsAnalyzing)
                return;

            IsAnalyzing= true;
            // Run native parsing in background
            bool success = true;
            await Task.Run(() =>
            {
                success = _analyzer.LoadFile(FilePath);
                StatusMessage = "Analyzing log file...";
            });

            if (!success)
            {
                IsAnalyzing = false;
                MessageBox.Show("Unable to load file.",
                                "Error",
                                MessageBoxButton.OK,
                                MessageBoxImage.Error);
                return;
            }

            int malformed = _analyzer.GetMalformedLineCount();

            if (malformed > 0)
            {
                MessageBox.Show(
                    $"File contains {malformed} malformed lines.\nPlease verify log format.",
                    "Invalid Format",
                    MessageBoxButton.OK,
                    MessageBoxImage.Warning);
                IsAnalyzing = false;
                StatusMessage = "Invalid log file format.";
                return;
            }


            // Get results
            InfoCount = _analyzer.GetInfoCount();
            WarnCount = _analyzer.GetWarnCount();
            ErrorCount = _analyzer.GetErrorCount();

            ModuleErrors.Clear();

            var moduleDict = _analyzer.GetModuleErrorCounts();

            foreach (var pair in moduleDict)
            {
                ModuleErrors.Add(new ModuleError
                {
                    Module = pair.Key,
                    Count = pair.Value
                });
            }
            IsAnalyzing = false;

            OnPropertyChanged(nameof(InfoCount));
            OnPropertyChanged(nameof(WarnCount));
            OnPropertyChanged(nameof(ErrorCount));
            StatusMessage = "Analysis complete.";
            CreatePieChart();
            OnPropertyChanged(nameof(PieModel));
        }

        public event PropertyChangedEventHandler ?PropertyChanged;
        private void OnPropertyChanged(string name)
        {
            PropertyChanged?.Invoke(this,
                new PropertyChangedEventArgs(name));
        }
        private void Reset()
        {
            ModuleErrors.Clear();
            InfoCount = 0;
            WarnCount = 0;
            ErrorCount = 0;
            OnPropertyChanged(nameof(InfoCount));
            OnPropertyChanged(nameof(WarnCount));
            OnPropertyChanged(nameof(ErrorCount));
        }

        public void ResetPieChart()
        {
            PieModel = new PlotModel
            {
                Title = "Log Distribution",
                TextColor = OxyColors.White
            };

            OnPropertyChanged(nameof(PieModel));
        }
    }
}
