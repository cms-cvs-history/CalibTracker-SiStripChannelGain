{
  TFile *_file0 = TFile::Open("output_histograms.root");
  double nr_of_entries = ChargeAPVPair_436306436_0->GetEntries();
  double mean_of_histogram = ChargeAPVPair_436306436_0->GetMean();
  double rms_of_histogram = ChargeAPVPair_436306436_0->GetRMS();
  TF1 *landaufit = new TF1("landaufit","landau",0.,450.);
  landaufit->SetParameters(nr_of_entries,mean_of_histogram,rms_of_histogram);
  ChargeAPVPair_436306436_0->Draw();
  //ChargeAPVPair_436306436_0->Rebin(40);
  ChargeAPVPair_436306436_0->Fit("landaufit");
  //TF1 * fitfunction = (TF1*) ChargeAPVPair_436306436_0->GetListOfFunctions()->First();
}
